#ifndef _UTILS_H_
#define _UTILS_H_

#include <cv.h>
#include <highgui.h>
//#include "feature.hpp"

typedef unsigned char uint8;

void show_image(const IplImage *src, const char *name);

/*
 * get y component of a image
 */
IplImage *get_component_Y(const IplImage *src);

IplImage *get_gray(const IplImage *src);
unsigned char get_img_val(const IplImage *src, int w, int h);
float pixval32f(const IplImage *src, int w, int h);
void set_pixval32f(const IplImage *src, int w, int h, float val);
void set_img_val(IplImage *src, int w, int h, unsigned char val);
unsigned char *iplImage2uint8point(IplImage *src);
IplImage *yuv2iplImage(const uint8* data, int w, int h);
void show_image_from_path(const char *path, const char *pathB);
void show_image_from_path(const char *path);
bool load_jpg_image(const char *path, int &w, int &h, uint8*, int);
bool load_image_with_color(const char *path, int &w, int &h, uint8 *data, int len);
void show_yuv(const uint8* data, int w, int h);

void show_yuv_and_path(const uint8* data, int w, int h, const char *path);

void drawPoint(IplImage *src, int w, int h);
void drawPoint( IplImage *src, int w, int h, int color );
void show_yuv(const char *path);

inline void show_mat(const cv::Mat &pic) {
    cv::namedWindow("mat", CV_WINDOW_AUTOSIZE);
    cv::imshow("mat", pic);
    cv::waitKey(0);
}

inline void resize_mat_by_width(const cv::Mat &src, cv::Mat &dst) {
    const int kmaxcols = 480;
    if (src.cols <= kmaxcols) {
        dst = src;
    } else {
        float factor = kmaxcols / (float)src.cols;
        cv::resize(src, dst, cv::Size(), factor, factor);
    }
}
#endif
