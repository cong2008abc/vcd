#include "feature_db.h"
#include "info_db.h"
#include "global.h"
#include "utils.h"
#include "imitation.h"
#include "feature/common.h"
#include "base/vcd_dir.h"
#include "gtest/gtest.h"
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
char db_path[] = "../feature_om/";
unsigned char *img_data;
int kMaxSize;

vcd::FeatureDB *db;
vcd::InfoDB *info_db_query;
vcd::InfoDB *info_db_src;

int open_db() {
    db = new vcd::FeatureDB();
    db->OpenDB(Global::lib_path, Global::query_method);

    info_db_query = new vcd::InfoDB();
    info_db_query->OpenDB(Global::info_db_query);

    info_db_src = new vcd::InfoDB();
    info_db_src->OpenDB(Global::info_db_src);

    return 0;
}

int query(const vcd::OM *feat_query) {
    const vcd::OM *feat_ret;
    float ret = db->Query(feat_query, &feat_ret);

    if (feat_ret == NULL) {
        printf("## -1.0 null\n");
        return 1;
    }

    vcd::uint64 key = feat_ret->GetID();
//    printf("%llu %llu\n", MAIN_KEY(key), FRAME_ID(key));
    if (MAIN_KEY(key) == 2) {
        printf("## %f %s\n", ret, info_db_src->GetItem(FRAME_ID(key)));
        //show_image_from_path(info_db_src->GetItem(FRAME_ID(key)));
    } else {
        printf("## %f null\n", ret);
    }

    return 0;
}

void test_method(const char *path, int om_type) {
    vcd::Dir feat_dir;    
    feat_dir.OpenDir(path);
    std::string feature_path;
    int k = 0;
    while (true) {
        if (feat_dir.GetNextFile(&feature_path) == false) {
            break;
        }
            printf("%s\n", feature_path.c_str());

        FILE *pf = fopen(feature_path.c_str(), "rb");
        if (pf == NULL) {
            printf("Open Feature File Error! %s\n", feature_path.c_str());
            continue;
        } else {
            //printf("%s\n", feature_path.c_str());
        }

        while (true) {
            vcd::OM *feat = vcd::ReadFeatureFromFile(pf, om_type);
            if (feat == NULL) {
                break;
            }

            vcd::uint64 key = feat->GetID();
            printf(">>%s\n", info_db_query->GetItem(FRAME_ID(key)));
            //show_image_from_path(info_db_query->GetItem(FRAME_ID(key)));
            query(feat);

            k++;
            printf("%??? %d\n", k);

//            if (k > 100)
//                break;
        }                
        fclose(pf);
    }
    printf("%d\n", k);
    
}

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

        //query_image(img_data, w, h);
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

TEST(query, om) {
    test_method(Global::query_path, Global::query_method);
}

int main(int argc, char **argv) {
//    open_db(); 
//    test_method(Global::query_path, Global::query_method);

    testing::InitGoogleTest(&argc, argv);
    open_db();
    return RUN_ALL_TESTS();
}
