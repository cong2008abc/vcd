#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "gtest/gtest.h"
#include "feature/feature.h"
#include "feature/om_feature.h"
#include "base/vcd_dir.h"
#include <string>
#include "imitation.h"

vcd::FeatureDB *db;
vcd::InfoDB *info_db;

void init() {
    db = new vcd::FeatureDB();
    info_db = new vcd::InfoDB();
}

//
// frame_process function will be callback by video decode process
// frame_idx records the times
static int frame_idx = 0;
static int feat_num = 0;
static int video_idx = 0;
FILE *pf[3];
const char *prefix = "/mnt/book/part";

int frame_process(unsigned char *data, int w, int h)
{
    if (frame_idx++ % 7 != 0) {
        feat_num++;
        if (feat_num % 100 == 0) {
            printf("%d\n", feat_num);
        }
        return 0;
    }
    vcd::uint32 feature_idx = (video_idx << 16) | frame_idx;
    vcd::OM *om[3];

    om[0] = vcd::SimplyOM::Extract(data, w, h, 8);
    om[1] = vcd::ImprovedOM::Extract(data, w, h, 8);
    om[2] = vcd::ImprovedOM::ExtractWithSaliency(data, w, h, 8);

    for (int i = 0; i < 3; i++) {
        om[i]->SetID(feature_idx);
        om[i]->DumpToFile(pf[i]);

        delete om[i];
    }

    //for (int i = 0; i < 3; i++)
    //    delete om[i];


    return 0;
}

void extract_video(const char *video_dir)
{
    vcd::Dir dir;
    dir.OpenDir(video_dir);
    std::string file;
    int file_idx = 0;
    while (1) {
        if (dir.GetNextFile(&file) == false)
            break;

        frame_idx = 0;
        feat_num = 0;
        char name[128];
        for (int i = 0; i < 3; i++) {
            sprintf(name, "../feature_om/%d_%d_%d.smp", video_idx, file_idx, i);
            pf[i] = fopen(name, "w");

            if (pf[i] == NULL) {
                fprintf(stderr, "Something Terrible happens\n");
            }
        }

        printf("processing %s\n", file.c_str());
        vcd::Imitation::ProcessVideo(file.c_str(), frame_process);
        printf("get %d frames\n", feat_num);

        for (int i = 0; i < 3; i++) {
            fclose(pf[i]);
            pf[i] = NULL;
        }

        file_idx++;
    }
}

void extract_video(int start_idx, int n) {
    video_idx = start_idx;
    for (int i = 0; i < n;i ++) {
        char dir_name[128];
        sprintf(dir_name, "%s%d", prefix, video_idx);

        extract_video(dir_name);

        video_idx++;
    }
}

void process_jpg_dir(const char *path) {
    vcd::Dir dir;
    dir.OpenDir(path);
    std::string file;
    int w, h;
    unsigned char *buf = new unsigned char[1024 * 1024 * 3];
    //int i = 0;
    while (1) {
        if (dir.GetNextFile(&file) == false) break;
        
        if (!simulate_input(file.c_str(), buf, 1024 * 1024 * 3, &w, &h)) {
            continue;
        }

        printf("%s\n", file.c_str());

        vcd::ImpOMFeature *impOM = new vcd::ImpOMFeature;
        vcd::uint32 feature_idx = info_db->Insert(file.c_str());
        impOM->ExtractFrame(buf, w, h);
        impOM->SetKeyId(feature_idx);
    }

    delete [] buf;
}

TEST(extract, video) {
    extract_video(364,1);
}

//TEST(speed, extract) {
//    init();
//    process_jpg_dir("/mnt/share/image_db");
//    vcd::Dir dir;
//    dir.OpenDir("/mnt/share/image_db");
//    std::string subdir;
//    while (1) {
//        if (dir.GetNextFile(&subdir) == false)
//            break;
//        
//        if (subdir.find('.') != std::string::npos)
//            continue;
//
//        printf("%s\n", subdir.c_str());
//        process_jpg_dir(subdir.c_str());
//    }
//
//    db->Dump("../feature_om");
//    info_db->Dump("../info/info.db");
    //process_jpg_dir("/mnt/db/all");
//}

void t() {
    vcd::FeatureDB f_db;
    vcd::InfoDB i_db;

    f_db.OpenDB("../feature_db");
    i_db.OpenDB("../info/info.db");
//    i_db.print();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
//    t();
}
