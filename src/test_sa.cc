#include "utils.h"
//#include "feature/saliency.h"
#include "feature/img_saliency.h"
#include "base/logging.h"
#include "sa_cvpr_09.h"
#include <stdio.h>
#include <dirent.h>
#include <highgui.h>

const int LIB_NUM = 2;
const char *lib[] = {"../img/",
                     "/mnt/db/sample/",
                     "/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};
const int kMaxSize = 1024 * 1024 * 3;
uint8 img_data[kMaxSize];
uint8 buff[1024 * 960 * 3];

/*
void test(const char *path) {
    if (path == NULL) {
        fprintf(stderr, "Path Error!\n");
        return;
    }  

    int w, h;
    if (!load_jpg_image(path, w, h, img_data, kMaxSize)) {
        fprintf(stderr, "Load Image Error!\n");
        return;
    }

    vcd::Saliency sa;
    uint8 *result = new uint8[w * h];
    //sa.ExtractArea(img_data, w, h, result);
    sa.ExtractSaliency(img_data, w, h, result);

    //with pause!
    show_yuv(img_data, w, h);
    show_yuv(result, w, h);

    delete [] result;
}
*/

void test_cvpr(const char *path) {
    if (path == NULL) {
        return;
    } 

    IplImage *src = cvLoadImage(path);
    if (src == NULL) {
        return;
    }

    vcd::cvpr09_saliency(src);

    cvReleaseImage(&src);
}

void test_hc_method(const char *path) {
    if (path == NULL) {
        return ;
    }

    cv::Mat _src = cv::imread(path, 1);
    VLOG(0, "_src dim: %d", _src.dims);

    if (!_src.data) {
        return;
    }
//    cv::cvtColor(_src, _src, CV_RGB2YUV);
//    show_mat(_src);
//   // uint8 *_show = (uint8*)(_src.data);
//    int h = _src.rows, w = _src.cols;
//    uint8 *_src_ptr = (uint8*)(_src.data);
//    uint8 *_show = new uint8[h * w];
//    int idx = 0;
//    for (int i = 0; i < h; ++i) {
//        for (int j = 0; j < w; ++j) {
//            //printf("%d %d %d %d\n",w*h, idx, i * w *3,  j * 3);
//            _show[idx++]= _src_ptr[i * w * 3 + j * 3 + 2];
//        }
//    }
//    printf("opk\n");
//    cv::Mat _single(h, w, CV_8U, _show);
//    show_mat(_single);

    //show_yuv(_show, _src.cols, _src.rows);

    cv::Mat src;
    resize_mat_by_width(_src, src, _src.cols);
//
//    VLOG(0, "check mat resize! before: %d %d after: %d %d\n",
//            _src.cols, _src.rows, src.cols, src.rows);
    show_mat(src);

    cv::Mat result;
    vcd::Saliency::Get(src, result);
    vcd::Saliency::Evaluate(result, result);

    if (!result.data) {
        return;
    }

    show_mat(result);
}

void test_dir(const char *path) {
    struct dirent **namelist;
    int file_num = scandir(path, &namelist, NULL, alphasort);

    char full_name[128];
    for (int i = 0; i < file_num; ++i) {
        sprintf(full_name, "%s%s", path, namelist[i]->d_name);
        printf("%s\n", full_name);

        test_hc_method(full_name);
    }
}

int main() {
    for (int i = 0; i < LIB_NUM; ++i) 
        test_dir(lib[i]);
}
