#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>
#include <chrono>
#include <map>

#include <opencv2/opencv.hpp>

#include <gflags/gflags.h>

#include "color_transfer.h"
#include "contrast.h"

using std::cin;
using std::cout;
using std::string;
using std::vector;
using namespace cv;

DEFINE_string(source, "", "source img path");
DEFINE_string(target, "", "target img path");
DEFINE_string(space, "LAB", "color space:LAB|CAM97|RGB|OPENCV_LAB");
DEFINE_bool(cluster, false, "if transfer color with cluster algorithm");
DEFINE_int32(k, 0, "k cluster if using cluster mode");
DEFINE_string(output, "", "output img path");

std::map<string, COLOR_SPACE> color_space_map = {
    {"LAB",LAB}, {"CAM97",CIECAM97}, {"RGB",RGB}, {"OPENCV_LAB",OPENCV_LAB}
};

void test_color_transfer(const Mat &source, const Mat &target) {
    Mat dst;
    Mat temp;
    Bgr2CIECAM97(source, temp);
    ShowFloatImg(temp, "source with CIECAM97");
    CIECAM972Bgr(temp, dst);
    ShowFloatImg(dst, "dst after CIECAM97 to BGR");
    Diff(source, dst, "diff after CIECAM97");
    ColorDiff(source, dst);
    ShowFloatImg(source, "source");
    ShowFloatImg(target, "target");
    // ClipImg(target, dst, source.size());
    ShowFloatImg(dst, "target after clip");
    // cout<<"source:"<<source<<"\n";
    Mat temp_source, temp_target;
    vector<float> stat_datas;
    Bgr2Lab(source, temp_source);
    // cout<<"temp source:"<<temp_source<<"\n";
    ShowFloatImg(temp_source, "temp source after bgr2lab");
    // cvtColor(source, dst, CV_BGR2Lab);
    // cout<<"source:"<<source<<"\tdst:"<<dst<<"\n";
    // ShowFloatImg(dst, "dst using CV_BGR2Lab");
    Lab2Bgr(temp_source, dst);
    // cout<<"temp source:"<<temp_source<<"\tdst:"<<dst<<"\n";
    ShowFloatImg(dst, "dst from temp source after lab2bgr");
    Diff(source, temp_source, "diff after LAB");
    ColorDiff(source, temp_source);
    Bgr2Lab(target, temp_target);
    ShowFloatImg(temp_target, "temp target after bgr2lab");
    Lab2Bgr(temp_target, dst);
    ShowFloatImg(dst, "dst from temp target after lab2bgr");
}

void test_segment() {
    Mat data = (Mat_<float>(2,2) << 1.0,2.0,3.0,4.0);
    Mat segment;
    GetSegment(data, 2, segment);
    cout<<"segment:"<<segment<<"\n";
    Mat labels = (Mat_<int>(2,2) << 0,1,2,0);
    vector<vector<float>> stat_data;
    CalStatisticData(data, labels, stat_data);
    if (stat_data.empty()) {
        LOGE("empty stat_data!\n");
        return;
    }
    int label_num = stat_data.size();
    int len = stat_data[0].size();
    for (int i = 0; i < label_num; i++) {
        for (int j = 0; j < len; j++) {
            cout<<stat_data[i][j]<<",";
        }
        cout<<"\n";
    }
}

int main(int argc, char *argv[])
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    LOGI("source:%s,target:%s,space:%s,cluster:%d,k:%d,output:%s\n",
    FLAGS_source.c_str(),FLAGS_target.c_str(),FLAGS_space.c_str(),FLAGS_cluster,FLAGS_k,FLAGS_output.c_str());
    // test_segment();
    if (FLAGS_source.empty() || FLAGS_target.empty())
    {
        LOGE("usage: %s [source_path] [target_path] [optional|output_path]\n", argv[0]);
        return -1;
    }
    if (color_space_map.find(FLAGS_space) == color_space_map.end()) {
        LOGE("Invalid color_space, please choose one in LAB|CAM97|RGB|OPENCV_LAB\n");
        return -1;
    }
    COLOR_SPACE color_space = color_space_map[FLAGS_space];
    string source_path(FLAGS_source);
    string target_path(FLAGS_target);
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
    // test_color_transfer(source, target);
    // ColorTransfer(source, target, dst);
    if (FLAGS_k > 0) {
        ContrastColorTransferWithCluster(source, target, dst, FLAGS_k, color_space);
    } else {
        ContrastColorTransfer(source, target, dst, color_space);
    }
    LOGI("source size(%d,%d),target size(%d,%d),dst size(%d,%d)\n", source.size().height, source.size().width, target.size().height, target.size().width, dst.size().height, dst.size().width);
    if (!FLAGS_output.empty())
    {
        // string output_path(argv[3]);
        imwrite(FLAGS_output, dst);
    }
    else
    {
        imshow("color transfer result", dst);
        waitKey();
    }
    return 0;
}