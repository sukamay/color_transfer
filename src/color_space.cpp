#include "color_space.h"


/* Bgr2CIECAM97: transfer image from RGB color space to L-alpha-beta color space
 * the L-alpha-beta color space is not the same in the opencv, cvtColor(img, dst, cv2::COLOR_Bgr2CIECAM97)
 * img: [in] source img
 * target: [output] dst
 * statistics_datas: [output] a vector of length 6, contain [l_avg, alpha_avg, beta_avg, l_sigma, alpha_sigma, beta_sigma]
 */
ERROR_CODE Bgr2CIECAM97(const Mat &img, Mat &dst)
{
    auto time_start = std::chrono::system_clock::now();
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    if (channels < 3 || img.type() != CV_32FC3)
    {
        return IMG_INVALID;
    }
    if (dst.size != img.size || dst.channels() != img.channels() || img.type() != dst.type())
    {
        dst = Mat(img.size(), img.type());
    }
    const float *p_img;
    float *p_dst;
    for (int i = 0; i < rows; i++)
    {
        p_img = img.ptr<float>(i);
        p_dst = dst.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float b = p_img[0];
            float g = p_img[1];
            float r = p_img[2];
            // float l = (0.3811 * r + 0.5783 * g + 0.0402 * b);
            // float m = (0.1967 * r + 0.7244 * g + 0.0782 * b);
            // float s = (0.0241 * r + 0.1288 * g + 0.8444 * b);

            // p_dst[0] = (2.00 * l + 1.00 * m + 0.05 * s);
            // p_dst[1] = (1.00 * l - 1.09 * m + 0.09 * s);
            // p_dst[2] = (0.11 * l + 0.11 * m - 0.22 * s);
            p_dst[0] = 0.9601 * r + 1.8874 * g + 0.2008 * b;
            p_dst[1] = 0.1688 * r - 0.1997 * g + 0.0310 * b;
            p_dst[2] = 0.0583 * r + 0.1150 * g - 0.1727 * b;
            LOGD("Bgr2CIECAM97(%d,%d),bgr(%f,%f,%f),CIECAM97(%f,%f,%f)\n", i, j, b, g, r, p_dst[0], p_dst[1], p_dst[2]);

            p_img += channels;
            p_dst += channels;
        }
    }
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("rgb to CIECAM97 done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

ERROR_CODE CIECAM972Bgr(const Mat &img, Mat &dst)
{
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (img.channels() < 3 || img.type() != CV_32FC3)
    {
        return IMG_INVALID;
    }
    if (img.size != dst.size || img.channels() != dst.channels() || img.type() != dst.type())
    {
        dst = Mat(img.size(), img.type());
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    const float *p_img;
    float *p_dst;
    for (int i = 0; i < rows; i++)
    {
        p_img = img.ptr<float>(i);
        p_dst = dst.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float a = p_img[0];
            float c1 = p_img[1];
            float c2 = p_img[2];

            p_dst[2] = 0.3277 * a + 3.6984 * c1 + 1.0438 * c2;
            p_dst[1] = 0.3281 * a - 1.8808 * c1 + 0.0444 * c2;
            p_dst[0] = 0.3289 * a - 0.0045 * c1 - 5.4074 * c2;
            LOGD("CIECAM972Bgr(%d,%d),CIECAM97(%f,%f,%f),bgr(%f,%f,%f)\n", i, j, p_img[0], p_img[1], p_img[2], p_dst[0], p_dst[1], p_dst[2]);

            p_img += channels;
            p_dst += channels;
        }
    }
    return OK;
}


/* Bgr2Lab: transfer image from RGB color space to L-alpha-beta color space
 * the L-alpha-beta color space is not the same in the opencv, cvtColor(img, dst, cv2::COLOR_BGR2LAB)
 * img: [in] source img
 * target: [output] dst
 * statistics_datas: [output] a vector of length 6, contain [l_avg, alpha_avg, beta_avg, l_sigma, alpha_sigma, beta_sigma]
 */
ERROR_CODE Bgr2Lab(const Mat &img, Mat &dst)
{
    auto time_start = std::chrono::system_clock::now();
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    if (channels < 3 || img.type() != CV_32FC3)
    {
        return IMG_INVALID;
    }
    if (dst.size != img.size || dst.channels() != img.channels() || img.type() != dst.type())
    {
        dst = Mat(img.size(), img.type());
    }
    const float *p_img;
    float *p_dst;
    for (int i = 0; i < rows; i++)
    {
        p_img = img.ptr<float>(i);
        p_dst = dst.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            float b = p_img[0];
            float g = p_img[1];
            float r = p_img[2];
            float l = (0.3811 * r + 0.5783 * g + 0.0402 * b);
            float m = (0.1967 * r + 0.7244 * g + 0.0782 * b);
            float s = (0.0241 * r + 0.1288 * g + 0.8444 * b);
            l = (l < 1e-4 ? 1 : l);
            m = (m < 1e-4 ? 1 : m);
            s = (s < 1e-4 ? 1 : s);
            l = log(l);
            m = log(m);
            s = log(s);

            p_dst[0] = (l + m + s) / sqrt(3.0);
            p_dst[1] = (l + m - 2 * s) / sqrt(6.0);
            p_dst[2] = (l - m) / sqrt(2.0);
            LOGD("bgr2lab(%d,%d),bgr(%f,%f,%f),lms(%f,%f,%f),lab(%f,%f,%f)\n", i, j, b, g, r, l, m, s, p_dst[0], p_dst[1], p_dst[2]);

            p_img += channels;
            p_dst += channels;
        }
    }
    auto time_end = std::chrono::system_clock::now();
    auto process_time = std::chrono::duration_cast<std::chrono::milliseconds>(time_end - time_start);
    LOGI("rgb to lab done, takes time: %lldms, %fs\n", process_time.count(), process_time.count() / 1e3);
    return OK;
}

ERROR_CODE Lab2Bgr(const Mat &img, Mat &dst)
{
    if (img.data == nullptr)
    {
        return IMG_EMPTY;
    }
    if (img.channels() < 3 || img.type() != CV_32FC3)
    {
        return IMG_INVALID;
    }
    if (img.size != dst.size || img.channels() != dst.channels() || img.type() != dst.type())
    {
        dst = Mat(img.size(), img.type());
    }
    int rows = img.rows;
    int cols = img.cols;
    int channels = img.channels();
    const float *p_img;
    float *p_dst;
    for (int i = 0; i < rows; i++)
    {
        p_img = img.ptr<float>(i);
        p_dst = dst.ptr<float>(i);
        for (int j = 0; j < cols; j++)
        {
            // Mat lab(Size(1, 3), CV_32F);
            // float *p_lab = lab.ptr<float>(0);
            // lab = LAB2LMS * lab;
            // for (int k = 0; k < 3; k++)
            // {
            //     p_lab[k] = exp(p_lab[k]);
            // }
            // lab = LMS2RGB * lab;
            // for (int k = 0; k < 3; k++)
            // {
            //     p_dst[k] = p_lab[k];
            // }
            float l = p_img[0] / sqrt(3.0) + p_img[1] / sqrt(6.0) + p_img[2] / sqrt(2.0);
            float m = p_img[0] / sqrt(3.0) + p_img[1] / sqrt(6.0) - p_img[2] / sqrt(2.0);
            float s = p_img[0] / sqrt(3.0) - 2.0f * p_img[1] / sqrt(6.0);
            l = exp(l);
            m = exp(m);
            s = exp(s);

            p_dst[2] = 4.4679 * l - 3.5873 * m + 0.1193 * s;
            p_dst[1] = -1.2186 * l + 2.3809 * m - 0.1624 * s;
            p_dst[0] = 0.0497 * l - 0.2439 * m + 1.2045 * s;
            LOGD("lab2bgr(%d,%d),lab(%f,%f,%f),lms(%f,%f,%f),bgr(%f,%f,%f)\n", i, j, p_img[0], p_img[1], p_img[2], l, m, s, p_dst[0], p_dst[1], p_dst[2]);

            p_img += channels;
            p_dst += channels;
        }
    }
    return OK;
}
