#include "imitation.h"
#include "utils.h"
#include "base/vcd_dir.h"
#include <stdlib.h>
#include <cv.h>

namespace vcd {

Imitation::Imitation() {
    _img_db = new Dir(); 
}

Imitation::~Imitation() {
    delete _img_db;
}

bool Imitation::OpenJpgDb(const char *path) {
    return _img_db->OpenDir(path);
}

bool Imitation::GetNextImg(uint8 *data, int buff_size, int *w, int *h) {
    std::string file;
    
    while (1) {
        // if no file next, return false
        if (_img_db->GetNextFile(&file) == false) {
            return false;
        }

        // if this file is not a pic, try next file
        fprintf(stderr, "%s\n", file.c_str());
        printf("%s\n", file.c_str());
        if (Imitation::ReadImg(file.c_str(), data, buff_size, w, h)) {
            break;
        }
    }
    return true;
}

bool Imitation::GetVideo(process_func func) {
    std::string file;
    while (1) {
        if (_img_db->GetNextFile(&file) == false) {
            break;
        }

        fprintf(stderr, "process %s\n", file.c_str());
        Imitation::ProcessVideo(file.c_str(), func);
    }
    return true;
}

int count;
bool Imitation::GetNextYUV(process_func func) {
    std::string file;
    count = 0;
    while (1) {
        if (_img_db->GetNextFile(&file) == false) {
            break;
        }
        fprintf(stderr, "process %s\n", file.c_str());
        Imitation::GetNextYUV(file.c_str(), func);
    }
    printf("total query image: %d\n", count);
    return true;
}

bool Imitation::GetNextYUV(const char *path, process_func func) {
    char tmp[128];
    strcpy(tmp, path);
    int len = strlen(path);
    int i,j;
    for (i = len - 1; i >= 0; i--) {
        if (path[i] == '-') break;
    }
    if (i < 0) return false;

    i++;
    j = i + 1;
    while (tmp[j] != 'x') j++;
    tmp[j] = 0;
    int w = atoi(&tmp[i]);
    i = j + 1;
    while (tmp[i] != '.') i++;
    tmp[i] = 0;
    int h = atoi(&tmp[j + 1]);

    if (w == 0 || h == 0) return false;
    
    int kbuf_size = w * h * 3 / 2;
    uint8 *buf = new uint8[kbuf_size];

    FILE *pf = fopen(path, "rb");
    int img_size = w * h * 3/2;
    int size;
    int pic_idx = 0;
    while (1) {
        size = fread(buf, sizeof(uint8), img_size, pf);
        if (size != img_size) break;
        printf("Pic %d: \n", pic_idx++);
        count++;
        (*func)(buf, w, h);

    } 

    delete [] buf;
    return true;
}

bool Imitation::ProcessVideo(const char *path, process_func func) {
    VideoProcessor pro; 
    return pro.ProcessVideo(path, func);
}

bool Imitation::ReadImg(const char *path, uint8 *data, int buff_size,
                        int *w, int *h) {
    return simulate_input(path, data, buff_size, w, h);
}

} // namespace vcd
