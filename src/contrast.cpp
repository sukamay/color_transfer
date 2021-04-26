#include "contrast.h"

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "utils.h"

using std::cin;
using std::cout;
using std::string;
using std::vector;
using namespace cv;

/* ContrastColorTransfer: color transfer from source to target
 * source: [in] source img
 * target: [in] target img, which should be the same size with source img
 * dst: [output] dst img
 */
ERROR_CODE ContrastColorTransfer(Mat &source, Mat &target, Mat &dst, COLOR_SPACE color_space)
{
    auto time_start = std::chrono::system_clock::now();
    if (source.data == nullptr || target.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (source.channels() != target.channels())
    {
        return IMG_INVALID;
    }
    int channels = source.channels();
    if (source.size != target.size)
    {
        ClipImg(target, dst, source.size());
        dst.copyTo(target);
    }
    if (dst.size != target.size)
    {
        dst = Mat(source.size(), CV_32FC3);
    }
    Mat temp_source, temp_target;
    vector<float> source_statistic_datas(6, 0.0);
    vector<float> target_statistic_datas(6, 0.0);
    ERROR_CODE ret;
    switch (color_space)
    {
    case LAB:
        ret = Bgr2Lab(source, temp_source);        
        if (ret != OK)
        {
            LOGE("Fail to transfer source from BGR to LAB! ret: %d\n", int(ret));
            return ret;
        }
        ret = Bgr2Lab(target, temp_target);
        if (ret != OK)
        {
            LOGE("Fail to transfer target from BGR to LAB! ret: %d\n", int(ret));
            return ret;
        }
        break;
    case CIECAM97:
        ret = Bgr2CIECAM97(source, temp_source);
        if (ret != OK)
        {
            LOGE("Fail to transfer source from BGR to CIECAM97! ret: %d\n", int(ret));
            return ret;
        }
        ret = Bgr2CIECAM97(target, temp_target);
        if (ret != OK)
        {
            LOGE("Fail to transfer target from BGR to CIECAM97! ret: %d\n", int(ret));
            return ret;
        }
        break;
    case RGB:
        temp_source = source.clone();
        temp_target = target.clone();
        break;
    case OPENCV_LAB:
        cvtColor(source, temp_source, COLOR_BGR2Lab);
        cvtColor(target, temp_target, COLOR_BGR2Lab);
        break;
    default:
        return COLOR_SPACE_INVALID;
    }
    CalStatisticData(temp_source, source_statistic_datas);
    CalStatisticData(temp_target, target_statistic_datas);
    // CIECAM97 to RGB
    int rows = source.rows;
    int cols = source.cols;
    float *p_source, *p_target;
    float *p_dst;
    vector<float> sigma_ratio(3);
    for (int k = 3; k < 6; k++)
    {
        sigma_ratio[k - 3] = target_statistic_datas[k] / source_statistic_datas[k];
    }
    // for debug
    LOGD("source stat data:(%f,%f,%f),(%f,%f,%f)\n", source_statistic_datas[0], source_statistic_datas[1], source_statistic_datas[2], source_statistic_datas[3], source_statistic_datas[4], source_statistic_datas[5]);
    LOGD("target stat data:(%f,%f,%f),(%f,%f,%f)\n", target_statistic_datas[0], target_statistic_datas[1], target_statistic_datas[2], target_statistic_datas[3], target_statistic_datas[4], target_statistic_datas[5]);
    for (int i = 0; i < rows; i++)
    {
        p_source = temp_source.ptr<float>(i);
        p_target = temp_target.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                p_target[k] = target_statistic_datas[k] + sigma_ratio[k] * (p_source[k] - source_statistic_datas[k]);
            }
            p_source += channels;
            p_target += channels;
        }
    }
    switch (color_space)
    {
    case LAB:
        Lab2Bgr(temp_target, dst);
        break;
    case CIECAM97:
        CIECAM972Bgr(temp_target, dst);
        break;
    case RGB:
        dst = temp_target.clone();
        break;
    case OPENCV_LAB:
        cvtColor(temp_target, dst, COLOR_Lab2BGR);
        break;
    default:
        return COLOR_SPACE_INVALID;
    }
    // ShowFloatImg(dst);
    dst.convertTo(dst, CV_8UC3, 255.0);
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("color transfer in CIECAM97 color space done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

/* ContrastColorTransferWithCluster: color transfer from source to target
 * source: [in] source img
 * target: [in] target img, which should be the same size with source img
 * dst: [output] dst img
 * K: [in] k clusters
 */
ERROR_CODE ContrastColorTransferWithCluster(Mat &source, Mat &target, Mat &dst, int K, COLOR_SPACE color_space)
{
    auto time_start = std::chrono::system_clock::now();
    if (source.data == nullptr || target.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (source.channels() != target.channels())
    {
        return IMG_INVALID;
    }
    if (K <= 0) {
        return ContrastColorTransfer(source, target, dst, color_space);
    }
    int channels = source.channels();
    if (source.size != target.size)
    {
        ClipImg(target, dst, source.size());
        dst.copyTo(target);
    }
    if (dst.size != target.size)
    {
        dst = Mat(source.size(), CV_32FC3);
    }
    Mat temp_source, temp_target, segment;
    vector<vector<float>> source_statistic_datas(K, vector<float>(6, 0.0));
    vector<vector<float>> target_statistic_datas(K, vector<float>(6, 0.0));
    GetSegment(target, K, segment);
    // segment = Mat(target.size(), CV_32S, Scalar(0))
    ERROR_CODE ret;
    switch (color_space)
    {
    case LAB:
        ret = Bgr2Lab(source, temp_source);        
        if (ret != OK)
        {
            LOGE("Fail to transfer source from BGR to LAB! ret: %d\n", int(ret));
            return ret;
        }
        ret = Bgr2Lab(target, temp_target);
        if (ret != OK)
        {
            LOGE("Fail to transfer target from BGR to LAB! ret: %d\n", int(ret));
            return ret;
        }
        break;
    case CIECAM97:
        ret = Bgr2CIECAM97(source, temp_source);
        if (ret != OK)
        {
            LOGE("Fail to transfer source from BGR to CIECAM97! ret: %d\n", int(ret));
            return ret;
        }
        ret = Bgr2CIECAM97(target, temp_target);
        if (ret != OK)
        {
            LOGE("Fail to transfer target from BGR to CIECAM97! ret: %d\n", int(ret));
            return ret;
        }
        break;
    case RGB:
        temp_source = source.clone();
        temp_target = target.clone();
        break;
    case OPENCV_LAB:
        cvtColor(source, temp_source, COLOR_BGR2Lab);
        cvtColor(target, temp_target, COLOR_BGR2Lab);
        break;
    default:
        return COLOR_SPACE_INVALID;
    }
    CalStatisticData(temp_source, segment, source_statistic_datas);
    CalStatisticData(temp_target, segment, target_statistic_datas);
    // CIECAM97 to RGB
    int rows = source.rows;
    int cols = source.cols;
    float *p_source, *p_target;
    float *p_dst;
    int *p_segment;
    vector<vector<float>> sigma_ratio(K, vector<float>(3));
    for (int i = 0; i < K; i++) {
        for (int k = 3; k < 6; k++)
        {
            sigma_ratio[i][k - 3] = target_statistic_datas[i][k] / source_statistic_datas[i][k];
        }
    }
    int label;
    for (int i = 0; i < rows; i++)
    {
        p_source = temp_source.ptr<float>(i);
        p_target = temp_target.ptr<float>(i);
        p_segment = segment.ptr<int>(i);
        for (int j = 0; j < cols; j++)
        {
            label = p_segment[j];
            for (int k = 0; k < 3; k++)
            {
                p_target[k] = target_statistic_datas[label][k] + sigma_ratio[label][k] * (p_source[k] - source_statistic_datas[label][k]);
            }
            p_source += channels;
            p_target += channels;
        }
    }
    switch (color_space)
    {
    case LAB:
        Lab2Bgr(temp_target, dst);
        break;
    case CIECAM97:
        CIECAM972Bgr(temp_target, dst);
        break;
    case RGB:
        break;
    case OPENCV_LAB:
        cvtColor(temp_target, dst, COLOR_Lab2BGR);
        break;
    default:
        return COLOR_SPACE_INVALID;
    }
    // ShowFloatImg(dst);
    dst.convertTo(dst, CV_8UC3, 255.0);
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("color transfer in CIECAM97 color space done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

void test_cam() {
    const Mat RGB2LMS = (Mat_<float>(3, 3) << 0.3811, 0.5783, 0.0402, 0.1967, 0.7244, 0.0782, 0.0241, 0.1288, 0.8444);
    const Mat LMS2RGB = (Mat_<float>(3, 3) << 4.4679, -3.5873, 0.1193, -1.2186, 2.3809, -0.1624, 0.0497, -0.2439, 1.2045);
    const Mat LMS2CAM = (Mat_<float>(3, 3) << 2.00, 1.00, 0.05, 1.00, -1.09, 0.09, 0.11, 0.11, -0.22);
    Mat RGB2CAM = LMS2CAM * RGB2LMS;
    cout<<"CAM="<<RGB2CAM<<" * RGB\n";
    Mat CAM2RGB;
    invert(RGB2CAM, CAM2RGB);
    cout<<"RGB="<<CAM2RGB<<" * CAM";
}