#include "om_interface.h"
#include "utils.h"
#include "imitation.h"
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
char db_path[] = "../feature_db/";
unsigned char *img_data;
int kMaxSize;

//void test_dir(const char *path) {
//    struct dirent **namelist;
//    int file_num = scandir(path, &namelist, NULL, alphasort);
//
//    char full_name[128];
//    for (int i = 0; i < file_num; ++i) {
//        sprintf(full_name, "%s%s", path, namelist[i]->d_name);
//        printf("%s\n", full_name);
//
//        int w, h;
//        if (!simulate_input(full_name, img_data, kMaxSize, &w, &h)) {
//            continue;
//        }
//
//        //printf("Load OK!\n");
//        query_image(img_data, w, h);
//    }
//}

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

        show_yuv(img_data, w, h);
        query_image(img_data, w, h);
    }
}

int main() {
    open_db(db_path); 
    kMaxSize = 1024 * 1024 * 3;
    img_data = new unsigned char[kMaxSize];

    for (int i = 0; i < LIB_NUM; ++i) {
        test_dir(lib[i]);
    }

    close_db();
    delete [] img_data;
}
