#ifndef _UTILS_H_
#define _UTILS_H_

#include <cv.h>
#include <highgui.h>
#include <string>
#include <vector>

typedef unsigned char   uint8;

/*
 * display functions
 */
void show_image(const IplImage *src, const char *name);
void show_image_from_path(const char *path, const char *pathB);
void show_image_from_path(const char *path);
void show_yuv(const char *path);
void show_yuv_colorful(const uint8 *data, int w, int h);
void show_yuv(const uint8* data, int w, int h);
void show_yuv(const uint8* data, int w, int h, cv::Rect &rect);
void show_yuv_and_path(const uint8* data, int w, int h, const char *path);
void show_query_result(const uint8 *data, int w, int h, const std::vector<std::string> &result);

inline void show_mat(const cv::Mat &pic, int second = 0) {
    cv::namedWindow("mat", CV_WINDOW_AUTOSIZE);
    cv::imshow("mat", pic);
    cv::waitKey(second);
}

/*
 * load img functions
 */
bool load_jpg_image(const char *path, int &w, int &h, uint8*, int);
bool load_image_with_color(const char *path, int &w, int &h, uint8 *data, int len);
bool load_img_by_mat(const char *, uint8*, int);
bool simulate_input(const char *, uint8*, int, int*, int*);

//IplImage *get_component_Y(const IplImage *src);
//IplImage *get_gray(const IplImage *src);
//unsigned char get_img_val(const IplImage *src, int w, int h);
//float pixval32f(const IplImage *src, int w, int h);
//void set_pixval32f(const IplImage *src, int w, int h, float val);
//void set_img_val(IplImage *src, int w, int h, unsigned char val);
//unsigned char *iplImage2uint8point(IplImage *src);
//IplImage *yuv2iplImage(const uint8* data, int w, int h);
//
//void drawPoint(IplImage *src, int w, int h);
//void drawPoint( IplImage *src, int w, int h, int color );


/*
 * other functions
 */
inline void resize_mat_by_width(const cv::Mat &src, cv::Mat &dst, int max_width = 480) {
    const int kmaxcols = max_width;
    if (src.cols <= kmaxcols) {
        dst = src;
    } else {
        float factor = kmaxcols / (float)src.cols;
        cv::resize(src, dst, cv::Size(), factor, factor);
    }
}

inline void resize_mat_by_height(const cv::Mat &src, cv::Mat &dst, int max_width = 480) {
    const int kmaxcols = max_width;
    if (src.rows <= kmaxcols) {
        dst = src;
    } else {
        float factor = kmaxcols / (float)src.rows;
        cv::resize(src, dst, cv::Size(), factor, factor);
    }
}

inline void draw_rectangle(cv::Mat &mat, const cv::Rect &rect) {
    cv::rectangle(mat, rect, cv::Scalar(0, 0, 255), 2);
}
    
#endif
