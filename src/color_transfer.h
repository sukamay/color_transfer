#ifndef COLOR_TRANSFER_H
#define COLOR_TRANSFER_H
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "error_code.h"
#include "log.h"
#include "utils.h"
#include "color_space.h"

using std::string;
using std::vector;
using namespace cv;

const Mat LMS2LAB1 = (Mat_<float>(3, 3) << 1.f / sqrt(3), 0, 0, 0, 1.f / sqrt(6), 0, 0, 0, 1.f / sqrt(2));
const Mat LMS2LAB2 = (Mat_<float>(3, 3) << 1, 1, 1, 1, 1, -2, 1, -1, 0);
const Mat LAB2LMS1 = (Mat_<float>(3, 3) << 1, 1, 1, 1, 1, -1, 1, -2, 0);
const Mat LAB2LMS2 = (Mat_<float>(3, 3) << sqrt(3) / 3.f, 0, 0, 0, sqrt(6) / 6.f, 0, 0, 0, sqrt(2) / 2.f);
const Mat LMS2RGB = (Mat_<float>(3, 3) << 0.3811, 0.5783, 0.0402, 0.1967, 0.7244, 0.0782, 0.0241, 0.1288, 0.8444);
const Mat RGB2LMS = (Mat_<float>(3, 3) << 4.4679, -3.5873, 0.1193, -1.2186, 2.3809, -0.1624, 0.0497, -0.2439, 1.2045);

ERROR_CODE ColorTransfer(Mat &source, Mat &target, Mat &dst);


#endif