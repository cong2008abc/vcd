#include "base/vcd_dir.h"
#include "imitation.h"
#include "utils.h"
#include "base/logging.h"
#include "feature/common.h"
#include "gtest/gtest.h"
#include <cv.h>
#include <highgui.h>
#include <string.h>
#include <time.h>

unsigned char *img_data;
int kMaxSize;

cv::Mat resize_img(const cv::Mat &src, float f1, float f2) {
    cv::Mat dst;
    cv::resize(src, dst, cv::Size(), f1, f2);
    return dst;
}

cv::Mat equ_hist(const cv::Mat &src, float alpha, int beta) {
    cv::Mat dst = cv::Mat::zeros(src.size(), src.type());

    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            for (int c = 0; c < 3; c++)
                dst.at<cv::Vec3b>(y,x)[c] = cv::saturate_cast<uchar>(
                                            alpha * (src.at<cv::Vec3b>(y,x)[c])
                                            + beta);
        }
    }

//    show_mat(dst);
    return dst;
}

cv::Mat insert_logo(const cv::Mat &src) {
    cv::Mat logo = cv::imread("../logo.png");

    //printf("%d\n", logo.nchannels);

    cv::Mat dst = src.clone();

    for (int y = 0; y < logo.rows; ++y) {
        for (int x = 0; x < logo.cols; ++x) {
            for (int c = 0; c < 3; ++c) {
                dst.at<cv::Vec3b>(y + 20, x + 10)[c] = logo.at<cv::Vec3b>(y,x)[c];
            }
        }
    }

    show_mat(dst);

    return dst;
}

cv::Mat margin(const cv::Mat &src) {
    cv::Mat dst = cv::Mat::zeros(cv::Size(src.cols + 40, src.rows + 80), src.type());

    for (int y = 0; y < src.rows; ++y) {
        for (int x = 0; x < src.cols; ++x) {
            for (int c = 0; c < 3; ++c) {
                dst.at<cv::Vec3b>(y + 40, x + 20)[c] = src.at<cv::Vec3b>(y,x)[c];
            }
        }
    }

  //  show_mat(dst);

    return dst;
}

cv::Mat noise_img(const cv::Mat &src, float factor) {
    cv::Mat noise = src.clone();

    int round = noise.cols * noise.rows * factor;
    while (round--) {
        int x = rand() % noise.cols;
        int y = rand() % noise.rows;

        for (int c = 0; c < 3; ++c)
            noise.at<cv::Vec3b>(y, x)[c] = 255;
    }
    //show_mat(noise);

    return noise;
}

cv::Mat cut_percent(const cv::Mat &src, float percent) {
    int n_w = src.cols * percent;
    int n_h = src.rows * percent;

    int left_up = (src.cols - n_w);
    int top_up = (src.rows - n_h);

    int left = rand() % left_up;
    int top = rand() % top_up;

    printf("%d %d\n", left,top);

    cv::Mat cur_img(src, cv::Rect(left, top, n_w, n_h));
    return cur_img.clone();
}

cv::Mat smooth(const cv::Mat &src, int size) {
    cv::Mat dst;
    cv::GaussianBlur(src, dst, cv::Size(size, size), 10, 10);
    return dst;
}

int get_num(const char *file_name) {
    int len = strlen(file_name);
    int i = 0;
    for (i = len - 1; i >= 0; i--) {
        if (file_name[i] == '/')
            break;
    }
    if (i < 0) return -1;

    int num = atoi(file_name + i + 1);
    return num;
}

void test_dir(const char *path) {
    kMaxSize = 1024 * 1024 * 3;
    img_data = new unsigned char[kMaxSize];
    std::string file_name;

    vcd::Dir dir;
    if (dir.OpenDir(path) == false) {
        fprintf(stderr, "no dir exists!\n");
        return;
    }

    srand(time(NULL));

    int w, h;
    while (1) {
        if (dir.GetNextFile(&file_name) == false) 
            break;

        cv::Mat src = cv::imread(file_name.c_str());
        if (!src.data) continue; 

        printf("%s\n", file_name.c_str());
        //show_mat(src);

//        noise_img(src);
        cv::Mat k[10];
        
//        k[0] = margin(src);
//        k[1] = equ_hist(src, 1.3,  3);
//        k[2] = equ_hist(src, 0.7,  3);
        k[5] = cut_percent(src, 0.75);
        k[4] = cut_percent(src, 0.80);
//        //k[4] = cut_percent(src, 0.8);
//        k[4] = smooth(src, 7);
//        k[5] = noise_img(src, 0.03);

        char new_name[64];
        int num = get_num(file_name.c_str());
        for (int i = 4; i < 6; ++i) {
            sprintf(new_name, "/mnt/share/image_lib/cut2/%d_%d.jpg", num, 11 + i);
            printf("%s\n", new_name);
            //show_mat(k[i]);
            cv::imwrite(new_name, k[i]);
        }


//
//        smooth(src);
//        show_mat(cur_img_09);
//        show_mat(cur_img_08);
        //show_yuv_colorful(img_data, w, h);
        //query_image(img_data, w, h);
    }
}

TEST(dir, dir) {
    test_dir("/mnt/share/image_query");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
