
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>

#include <opencv2/opencv.hpp>

#include "color_transfer.h"

using std::cin;
using std::cout;
using std::string;
using std::vector;
using namespace cv;

int TestColorTransfer(int argc, char *argv[])
{
    if (argc < 3)
    {
        LOGE("usage: %s [source_path] [target_path] [optional|output_path]\n", argv[0]);
        return -1;
    }
    string source_path(argv[1]);
    string target_path(argv[2]);
    Mat img = imread(source_path);
    Mat source;
    Mat target;
    if (img.data == nullptr)
    {
        LOGE("Fail to open source img:%s!\n", source_path.c_str());
        return -1;
    }
    img.convertTo(source, CV_32FC3, 1.0f / 255.0f);
    img = imread(target_path);
    if (img.data == nullptr)
    {
        LOGE("Fail to open target img:%s!\n", target_path.c_str());
        return -1;
    }
    img.convertTo(target, CV_32FC3, 1.0f / 255.0f);
    assert(target.type() == CV_32FC3);
    Mat dst;
    ColorTransfer(source, target, dst);
    LOGI("source size(%d,%d),target size(%d,%d),dst size(%d,%d)\n", source.size().height, source.size().width, target.size().height, target.size().width, dst.size().height, dst.size().width);
    if (argc > 3)
    {
        string output_path(argv[3]);
        imwrite(output_path, dst);
    } else {
        imshow("color transfer result", dst);
        waitKey();
    }
    return 0;
}