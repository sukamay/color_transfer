#ifndef COLOR_SPACE_H
#define COLOR_SPACE_H

#include <opencv2/opencv.hpp>

#include "error_code.h"
#include "log.h"

using namespace cv;

ERROR_CODE Bgr2CIECAM97(const Mat &img, Mat &dst);
ERROR_CODE CIECAM972Bgr(const Mat &img, Mat &dst);
ERROR_CODE Bgr2Lab(const Mat &img, Mat &dst);
ERROR_CODE Lab2Bgr(const Mat &img, Mat &dst);

#endif
