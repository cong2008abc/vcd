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
