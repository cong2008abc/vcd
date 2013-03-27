#include "define.h"
#include "image_buf.h"
#include "utils.h"
#include "base/vcd_dir.h"
#include <string>
#include <set>
#include <stdio.h>
#include <string.h>

//#ifndef uint64
//typedef unsigned long long uint64;
//#endif
typedef unsigned char uint8;
const int kbufsize = 1024 * 1024 * 10;      // for test, 10MB buf
int total_repeat = 0;

void process_imgdb(const char *path, const std::set<uint64> &reap_feat) {
    FILE *pf = fopen(path, "rb");
    if (pf == NULL) {
        return;
    }
    char name[128];
//    strcpy(name, path);
//    strcat(name, "_n");
//    FILE *wpf = fopen(name, "wb");
//    if (wpf == NULL) {
//        fclose(pf);
//        return;
//    }

    vcd::image_node img;
    int buf_idx = 0;
    int node_num = 0;
    unsigned char img_buf[1024 * 960 * 3];
    while (1) {
         int res = fread(&node_num, sizeof(int), 1, pf);
         if (res != 1) break;
         for (int i = 0; i < node_num; ++i) {
            res = fread(&img, sizeof(vcd::image_node), 1, pf);
            if (res != 1) {
                perror("fwrite");
                //printf("%d\n", feof(pf));
                printf("something wrong in read img node %d %d %d\n", i, node_num, buf_idx);
                break;
                //continue;
            }

            int img_size = img.w * img.h * 3 / 2;
            if (reap_feat.count(img.om_16i) != 0) {
                sprintf(name, "../repeat_image/%llu.yuv", img.om_16i);
                FILE *wpf = fopen(name, "ab");
                res = fread(img_buf, sizeof(uint8), img_size, pf);
                if (res != img_size) break;
                fwrite(&img, sizeof(vcd::image_node), 1, wpf);
                fwrite(img_buf, sizeof(uint8), 1, wpf);
                fclose(wpf);
                total_repeat++;
                //show_yuv_colorful(img_buf, img.w, img.h);
            } else {
                fseek(pf, img_size, SEEK_CUR); 
            }
         }
         buf_idx++;
         fseek(pf, buf_idx * kbufsize, SEEK_SET);
    }
    printf("%d\n", buf_idx);
    fclose(pf);
//    fclose(wpf);
}

void read_omf_file(const char *path, std::set<uint64> *reap_feat,
                   std::set<uint64> *all_feat) {
    FILE *pf = fopen(path, "rb");
    if (pf == NULL) {
        printf("error\n");
        return;
    }

    unsigned long long cprs_feat = 0;
    int real[16];
    const int mask =  (1 << 4) - 1;
    while (1) {
        int res = 0;
        if ((res = fread(&cprs_feat, sizeof(unsigned char), 8, pf)) != 8) {
            break;
        }

        if (all_feat->count(cprs_feat) == 0) {
            all_feat->insert(cprs_feat);
        } else {
            reap_feat->insert(cprs_feat);
        }
        //printf("%llu\n", cprs_feat);
    }

    fclose(pf);
}

void read_file() {
    vcd::Dir dir;
    dir.OpenDir("/mnt/1/");
    std::string filename;

    std::set<uint64> all_feat, reap_feat;

    while (true) {
        if (dir.GetNextFile(&filename) == false) {
            break;
        }

        if (filename.find("omf") == std::string::npos) {
            continue;
        }

        printf("%s\n", filename.c_str());
        read_omf_file(filename.c_str(), &reap_feat, &all_feat);
    }
    printf("%d\n", reap_feat.size());

    dir.Reset();
    while (true) {
        if (dir.GetNextFile(&filename) == false) {
            break;
        } 
        if (filename.find("tmp.imgdb") != filename.size() - 9) {
            continue;
        }
        printf("%s\n", filename.c_str());
        process_imgdb(filename.c_str(), reap_feat);
    }
    printf("total repeat:%d\n", total_repeat);
}

int main() {
    read_file();
}
