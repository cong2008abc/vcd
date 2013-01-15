#include "om_interface.h"
#include "utils.h"
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

const int LIB_NUM = 3;
const char *lib[] = {"/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};

unsigned char *img_data;
int kMaxSize;

void test_dir(const char *path) {
    struct dirent **namelist;
    int file_num = scandir(path, &namelist, NULL, alphasort);

    char full_name[128];
    for (int i = 0; i < file_num; ++i) {
        sprintf(full_name, "%s%s", path, namelist[i]->d_name);
        printf("%s\n", full_name);

        int w, h;
        if (!load_jpg_image(full_name, w, h, img_data, kMaxSize)) {
            continue;
        }

        //printf("Load OK!\n");
        query_image(img_data, w, h);
    }
}

int main() {
    open_db("../test_om/"); 
    kMaxSize = 1024 * 1024 * 3;
    img_data = new unsigned char[kMaxSize];

    for (int i = 0; i < LIB_NUM; ++i) {
        test_dir(lib[i]);
    }

    close_db();
    delete [] img_data;
}
