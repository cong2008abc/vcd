#include "om_interface.h"
#include "utils.h"
#include "imitation.h"
#include "feature/common.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cv.h>

const int LIB_NUM = 3;
const char *lib[] = {"../img/",
                     "/mnt/db/sample/",
                     "/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};
char db_path[] = "../feature_db2/";
unsigned char *img_data;
int kMaxSize;

void test_dir(const char *path) {
    vcd::Imitation imgs;
    if (imgs.OpenJpgDb(path) == false) {
        fprintf(stderr, "no dir exists!\n");
        return;
    }

    int w, h;
    while (1) {
        if (imgs.GetNextImg(img_data, kMaxSize, &w, &h) == false) {
            break;
        }

        query_image(img_data, w, h);
    }
}

int idx = 0;
int save_yuv(unsigned char *data, int w, int h) {
    cv::Mat img;
    vcd::cvt_YUV2RGB(data, w, h, &img);
    char name[32];
    sprintf(name, "/mnt/share/queryYUV/%d.jpg", idx++);
    imwrite(name, img);
    return 0;
}

void test_yuv(const char *path) {
    vcd::Imitation imi;
    imi.OpenJpgDb(path);
    imi.GetNextYUV(save_yuv);
//    imi.GetNextYUV(query_image);
}

int main(int argc, char **argv) {
    open_db(db_path); 
    kMaxSize = 1024 * 1024 * 3;
    img_data = new unsigned char[kMaxSize];
    test_dir(argv[1]);
//
//    for (int i = 0; i < LIB_NUM; ++i) {
//        test_dir(lib[i]);
//    }

    close_db();
//    delete [] img_data;
}
