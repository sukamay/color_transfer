#ifndef CONTRAST_H
#define CONTRAST_H
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "error_code.h"
#include "log.h"
#include "struct_define.h"
#include "color_space.h"

using std::string;
using std::vector;
using namespace cv;

ERROR_CODE ContrastColorTransfer(Mat &source, Mat &target, Mat &dst, COLOR_SPACE color_space = LAB);
ERROR_CODE ContrastColorTransferWithCluster(Mat &source, Mat &target, Mat &dst, int K, COLOR_SPACE color_space = LAB);
void test_cam();

#endif