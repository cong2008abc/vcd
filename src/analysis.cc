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
uint8 *img_buf;
int total_repeat = 0;

void process_imgdb(const char *path, const std::set<uint64> &reap_feat,
                   std::set<uint64> *feat) {
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
    //unsigned char img_buf[1024 * 960 * 3];
    while (1) {
//        int res = fread(img_buf, sizeof(uint8), kbufsize, pf);
//        if (res != kbufsize) {
//            printf("something wrong in read img node %d %d\n", buf_idx, res);
//            break;
//        }
//        int node_num = *((int*)img_buf);
//        //printf("%d\n", node_num);
//        uint8 *ptr = img_buf + sizeof(int);
//        for (int i = 0; i < node_num; ++i) {
//            img = (vcd::image_node*)ptr;
//
//            feat->insert(img->om_16i);
//            int img_size = img->w * img->h * 3 / 2;
//            ptr = ptr + sizeof(vcd::image_node) + img_size;
//            if (ptr > img_buf + kbufsize) {
//                printf("%d,%d %d %d\n", img->w, img->h, i, node_num);
//                break;
//            }
//        }

//
         int res = fread(&node_num, sizeof(int), 1, pf);
         if (res != 1) break;
         for (int i = 0; i < node_num; ++i) {
            res = fread(&img, sizeof(char), sizeof(vcd::image_node), pf);
            if (res != sizeof(vcd::image_node)) {
//                printf("%d\n", res);
//                perror("fwrite");
//                printf("%d\n", feof(pf));
//                printf("something wrong in read img node %d %d %d\n", i, node_num, buf_idx);
                break;
                continue;
            }

            int img_size = img.w * img.h * 3 / 2;
//            feat->insert(img.om_16i);
//            fseek(pf, img_size, SEEK_CUR); 
//            continue;

            if (reap_feat.count(img.om_16i) != 0) {
                res = fread(img_buf, sizeof(uint8), img_size, pf);
                if (res == img_size) {
                    sprintf(name, "../repeat_image/%llu.yuv", img.om_16i);
                    FILE *wpf = fopen(name, "ab");
                    fwrite(&img, sizeof(vcd::image_node), 1, wpf);
                    fwrite(img_buf, sizeof(uint8), img_size, wpf);
                    fclose(wpf);
                    total_repeat++;
                }
//                show_yuv_colorful(img_buf, img.w, img.h);
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

void show_yuv_file(const char *path) {
    printf("%s\n", path);
    FILE *pf = fopen(path, "rb");
    if (pf == NULL) {
        perror("fopen");
        return;
    }

    while (1) {
        vcd::image_node node;
        int res = fread(&node, sizeof(node), 1, pf);
        if (res != 1) break;
        fread(img_buf, sizeof(uint8), node.w * node.h * 3 / 2, pf);

        show_yuv_colorful(img_buf, node.w, node.h);
        //break;
    }
    fclose(pf);

}

void show_yuv_file() {
    vcd::Dir dir;
    dir.OpenDir("../repeat_image/");
    std::string image_name;
    //uint8 data[1024 * 960 * 3];
    while (1) {
        if (dir.GetNextFile(&image_name) == false) {
            break;
        }

        if (image_name.find("yuv") == std::string::npos) {
            continue;
        }

        show_yuv_file(image_name.c_str());
    }
}

void read_finish_imgdb() {
    vcd::Dir dir;
    dir.OpenDir("/mnt/1/");
    std::string filename;

    while (true) {
        if (dir.GetNextFile(&filename) == false) {
            break;
        }

        if (filename.find("tmp.imgdb") == filename.size() - 9 ||
            filename.find("imgdb") != filename.size() - 5) {
            continue;
        }

        FILE *pf = fopen(filename.c_str(), "rb");
        if (pf == NULL) continue;

        printf("%s\n", filename.c_str());
        vcd::image_node node;
        int idx = 0;
        while (1) {
            int res = fread(&node, sizeof(node), 1, pf);
            if (res != 1) break;

            int image_size = node.w * node.h * 3/2;
            if (image_size > 1024 * 800) {
                continue;
            }
            printf("%d\n", idx++);
            fread(img_buf, sizeof(uint8), image_size, pf);

            char yuv_name[128];
            sprintf(yuv_name, "../yuv/%llu.yuv", node.om_16i);
            FILE *wpf = fopen(yuv_name, "ab");
            fwrite(&node, sizeof(node), 1, wpf);
            fwrite(img_buf, sizeof(uint8), image_size, wpf);
            fclose(wpf);

            //show_yuv_colorful(img_buf, node.w, node.h);
        }
        fclose(pf);
    }
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

    std::set<uint64> img_feat;
    dir.Reset();
    while (true) {
        if (dir.GetNextFile(&filename) == false) {
            break;
        } 
        if (filename.find("tmp.imgdb") != filename.size() - 9) {
            continue;
        }
        printf("%s\n", filename.c_str());
        process_imgdb(filename.c_str(), reap_feat, &img_feat);
        printf("img feat count: %d\n", img_feat.size());
    }
    printf("total repeat:%d\n", total_repeat);
}

int main(int argc, char **argv) {
    img_buf = new uint8[kbufsize];
    show_yuv_file(argv[1]);
    //read_file();
    //show_yuv_file();
    //read_finish_imgdb();
}
