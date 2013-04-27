#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "gtest/gtest.h"
#include "feature/feature.h"
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

int frame_process(unsigned char *data, int w, int h)
{
    vcd::ImpOMFeature *impOM = new vcd::ImpOMFeature;
    vcd::uint32 feature_idx = info_db->Insert(file.c_str());
    impOM->ExtractFrame(buf, w, h);
    impOM->SetKeyId(feature_idx);
}

void extract_video(const char *video_dir)
{
    vcd::Dir dir;
    dir.OpenDir(video_dir);
    std::string file;
    while (1) {
        if (dir.GetNextFile(&file) == false)
            break;

        frame_idx = 0;
        Imitation::ProcessVideo(file.c_str(), frame_process);
    }
}

void process_jpg_dir(const char *path) {
    vcd::Dir dir;
    dir.OpenDir(path);
    std::string file;
    int w, h;
    unsigned char *buf = new unsigned char[1024 * 1024 * 3];
    int i = 0;
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

//        db->AddFeature(impOM);
//        if (++i > 10)
//        break;
    }

    delete [] buf;
}

TEST(speed, extract) {
    init();
    process_jpg_dir("/mnt/share/image_db");
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
}

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
