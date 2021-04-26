#include "color_transfer.h"

#include <iostream>
#include <stdio.h>
#include <cmath>
#include <chrono>

#include <opencv2/opencv.hpp>

using std::cin;
using std::cout;
using std::string;
using std::vector;
using namespace cv;

/* ColorTransfer: color transfer from source to target
 * source: [in] source img
 * target: [in] target img, which should be the same size with source img
 * dst: [output] dst img
 */
ERROR_CODE ColorTransfer(Mat &source, Mat &target, Mat &dst)
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
    CalStatisticData(temp_source, source_statistic_datas);
    CalStatisticData(temp_target, target_statistic_datas);
    // calculate l',alpha',beta'
    // LAB to LMS to RGB
    int rows = source.rows;
    int cols = source.cols;
    // if (source.isContinuous() && target.isContinuous()) {
    //     rows *= cols;
    //     cols = 1;
    // }
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
    // Mat LAB2LMS = LAB2LMS1 * LAB2LMS2;
    for (int i = 0; i < rows; i++)
    {
        p_source = temp_source.ptr<float>(i);
        p_target = temp_target.ptr<float>(i);
        p_dst = dst.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            // Mat lab(Size(1, 3), CV_32F);
            // float *p_lab = lab.ptr<float>(0);
            for (int k = 0; k < 3; k++)
            {
                // p_lab[k] = target_statistic_datas[k] + sigma_ratio[k] * p_source[k];
                p_target[k] = target_statistic_datas[k] + sigma_ratio[k] * (p_source[k] - source_statistic_datas[k]);
            }
            // lab = LAB2LMS * lab;
            // for (int k = 0; k < 3; k++)
            // {
            //     p_lab[k] = exp(p_lab[k]);
            // }
            // lab = LMS2RGB * lab;
            // LOGD("(%d,%d):source-rgb(%f,%f,%f),target-rgb(%f,%f,%f),target-lab(%f,%f,%f)", i, j, p_source[0], p_source[1], p_source[2],
            //      p_target[0], p_target[1], p_target[2], p_lab[0], p_lab[1], p_lab[2]);
            // for (int k = 0; k < 3; k++)
            // {
            //     p_dst[k] = (uchar)(p_lab[k] * 255.0f);
            // }
            // LOGD("target-rgb(%f,%f,%f)\n", p_dst[0], p_dst[1], p_dst[2]);
            p_source += channels;
            p_target += channels;
            p_dst += channels;
        }
    }
    Lab2Bgr(temp_target, dst);
    // ShowFloatImg(dst);
    dst.convertTo(dst, CV_8UC3, 255.0);
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("color transfer done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

