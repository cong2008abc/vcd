#include "utils.h"
//#include "feature/saliency.h"
#include "feature/img_saliency.h"
#include "base/logging.h"
#include "feature/om.h"
#include <stdio.h>
#include <dirent.h>
#include <highgui.h>

const int LIB_NUM = 2;
const char *lib[] = {
                     "../img/",
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

void test_hc_method(const char *path, cv::Mat *res) {
    if (path == NULL) {
        return ;
    }

    cv::Mat _src = cv::imread(path, 1);
    VLOG(0, "_src dim: %d", _src.dims);

    if (!_src.data) {
        return;
    }

//    cv::Rect margin;
//    remove_margin(_src, &margin);
//    cv::Mat roi(_src, margin);
    cv::Mat src;
    cv::Mat roi = _src;
    resize_mat_by_width(roi, src, 120);
    show_mat(src);

    cv::Mat result;
    vcd::Saliency::Get(src, result, vcd::Saliency::RC);
    //cv::imwrite("../img/sample_orig.jpg", src);
    vcd::Saliency::Evaluate(result, result);
    //cv::imwrite("../img/sample_rc01.jpg", result);

    if (!result.data) {
        return;
    }

    show_mat(result);
    *res = result;
}

void test_dir(const char *path) {
    struct dirent **namelist;
    int file_num = scandir(path, &namelist, NULL, alphasort);

    char full_name[128];
    for (int i = 2; i < file_num; ++i) {
        sprintf(full_name, "%s%s", path, namelist[i]->d_name);
        printf("%s\n", full_name);

        cv::Mat res;
        test_hc_method(full_name, &res);
//        if (res.data) {
//            sprintf(full_name, "../img/%s_bef", namelist[i]->d_name);
//            printf("%s\n", full_name);
//            cv::imwrite(full_name, res);
//        }
    }
}

int main() {
    printf("%s\n", CV_VERSION);
    return 0;
    for (int i = 0; i < LIB_NUM; ++i) 
        test_dir(lib[i]);
}
