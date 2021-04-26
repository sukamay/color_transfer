#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "error_code.h"
#include "log.h"

using std::string;
using std::vector;
using namespace cv;

ERROR_CODE CalStatisticData(const Mat &img, vector<float> &statistics_datas);
ERROR_CODE CalStatisticData(const Mat &img, const Mat &segment, vector<vector<float>> &statistics_datas);
ERROR_CODE ClipImg(Mat &img, Mat &dst, const Size &target_size);
ERROR_CODE GetSegment(const Mat &img, int K, Mat &segment);

ERROR_CODE Diff(const Mat &source, const Mat &target, string window_name = "diff");
ERROR_CODE ColorDiff(const Mat &source, const Mat &target, string window_name = "color_diff");

void ShowFloatImg(const Mat &img, string window_name = "dst");

#endif