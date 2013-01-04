#include "utils.h"
#include "feature/saliency.h"
#include "sa_cvpr_09.h"
#include <stdio.h>
#include <dirent.h>
#include <highgui.h>

const int LIB_NUM = 3;
const char *lib[] = {"/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};
const int kMaxSize = 1024 * 1024 * 3;
uint8 img_data[kMaxSize];

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
    // sa.ExtractArea(img_data, w, h, result);
    sa.ExtractSaliency(img_data, w, h, result);

    //with pause!
    show_yuv(result, w, h);

    delete [] result;
}

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

void test_dir(const char *path) {
    struct dirent **namelist;
    int file_num = scandir(path, &namelist, NULL, alphasort);

    char full_name[128];
    for (int i = 0; i < file_num; ++i) {
        sprintf(full_name, "%s%s", path, namelist[i]->d_name);
        printf("%s\n", full_name);

        test_cvpr(full_name);
    }
}

int main() {
    for (int i = 0; i < LIB_NUM; ++i) 
        test_dir(lib[i]);
}
