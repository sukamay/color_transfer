#include "utils.h"

using namespace cv;

// cols/width/x, rows/height/y
/* ClipImg: clip input img to target size
 * img: [in] input img
 * dst: [output] dst img
 * target_size: [in] target size 
 */
ERROR_CODE ClipImg(Mat &img, Mat &dst, const Size &target_size)
{
    auto time_start = std::chrono::system_clock::now();
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (img.data == dst.data || img.size != dst.size)
    {
        dst = Mat(target_size, CV_32FC3, Scalar(1.0f, 1.0f, 1.0f));
    }
    Size img_size = img.size();
    float ratio = fmin(float(target_size.width) / img_size.width, float(target_size.height) / img_size.height);
    Mat scale_img;
    Size scale_size(int(float(img_size.width) * ratio), int(float(img_size.height) * ratio));
    resize(img, scale_img, scale_size, 0, 0, INTER_LINEAR);
    Mat roi = dst(Rect(0, 0, scale_size.width, scale_size.height));
    scale_img.copyTo(roi);
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("clip img done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

/* CalStatisticData: calculate avg color and sigma per channel
 * img: [in] input img
 * statistics_datas: [out] length should be img.channels*2, [0:channels] records the avg, [channels:end] records sigma
 */
ERROR_CODE CalStatisticData(const Mat &img, vector<float> &statistics_datas)
{
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    if (statistics_datas.size() < channels * 2)
    {
        vector<float>().swap(statistics_datas);
        statistics_datas = vector<float>(channels * 2, 0.0f);
    }
    // calculate avg
    for (int i = 0; i < rows; i++)
    {
        const float *p_img = img.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < channels; k++)
            {
                statistics_datas[k] += p_img[k];
            }
            p_img += channels;
        }
    }
    float total_pixel_num = float(rows * cols);
    for (int k = 0; k < channels; k++)
    {
        statistics_datas[k] /= total_pixel_num;
    }
    // calculate sigma
    for (int i = 0; i < rows; i++)
    {
        const float *p_img = img.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < channels; k++)
            {
                statistics_datas[k + channels] += pow(p_img[k] - statistics_datas[k], 2);
            }
            p_img += channels;
        }
    }
    for (int k = 0; k < channels; k++)
    {
        statistics_datas[k + channels] = sqrt(statistics_datas[k + channels] / total_pixel_num);
    }
    return OK;
}


ERROR_CODE CalStatisticData(const Mat &img, const Mat &segment, vector<vector<float>> &statistics_datas) {
    if (img.data == nullptr || segment.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (img.size() != segment.size()) {
        return IMG_INVALID;
    }
    if (statistics_datas.empty()) {
        return PARAM_INVALID;
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    int range = statistics_datas.size();
    vector<int> label_counts(range, 0);
    int label = 0;
    // calculate avg
    for (int i = 0; i < rows; i++)
    {
        const float *p_img = img.ptr<float>(i);
        const int *p_segment = segment.ptr<int>(i);
        for (int j = 0; j < cols; j++)
        {
            label = p_segment[j];
            label_counts[label]++;
            for (int k = 0; k < channels; k++)
            {
                statistics_datas[label][k] += p_img[k];
            }
            p_img += channels;
        }
    }
    for (int i = 0; i < range; i++) {
        for (int k = 0; k < channels; k++)
        {
            statistics_datas[i][k] /= float(label_counts[i]);
        }
    }
    // calculate sigma
    for (int i = 0; i < rows; i++)
    {
        const float *p_img = img.ptr<float>(i);
        const int *p_segment = segment.ptr<int>(i);
        for (int j = 0; j < cols; j++)
        {
            label = p_segment[j];
            for (int k = 0; k < channels; k++)
            {
                statistics_datas[label][k + channels] += pow(p_img[k] - statistics_datas[label][k], 2);
            }
            p_img += channels;
        }
    }
    for (int i = 0; i < range; i++) {
        for (int k = 0; k < channels; k++)
        {
            statistics_datas[i][k + channels] = sqrt(statistics_datas[i][k + channels] / (float)label_counts[i]);
        }
    }
    return OK;
}

void ShowFloatImg(const Mat &img, string window_name)
{
    if (img.type() != CV_32FC3)
    {
        return;
    }
    double min_value, max_value;
    minMaxLoc(img, &min_value, &max_value);
    LOGI("img min value:%f, max value:%f\n", min_value, max_value);
    Mat dst;
    double ratio = 255.0 / (max_value - min_value);
    img.convertTo(dst, CV_8UC3, ratio, min_value / (min_value - max_value));
    imshow(window_name, dst);
    waitKey();
}

ERROR_CODE Diff(const Mat &source, const Mat &target, string window_name) {
    Mat dst;
    absdiff(source, target, dst);
    ShowFloatImg(dst, window_name);
    return OK;
}
ERROR_CODE ColorDiff(const Mat &source, const Mat &target, string window_name) {
    Mat dst;
    absdiff(source, target, dst);
    cv::pow(dst, 3.0, dst);
    ShowFloatImg(dst, window_name);
    return OK;
}

ERROR_CODE GetSegment(const Mat &img, int K, Mat &segment) {
    if (img.data == nullptr) {
        return IMG_EMPTY;
    }
    Mat samples;
    img.convertTo(samples, CV_32F);
    samples = samples.reshape(0, img.rows * img.cols);
    TermCriteria criteria(CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.1);
    Mat centers;
    int attemps = 3;
    kmeans(samples, K, segment, criteria, attemps, cv::KMEANS_RANDOM_CENTERS, centers);
    segment = segment.reshape(1, img.rows);
    return OK;
}
