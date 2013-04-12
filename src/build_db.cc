#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "gtest/gtest.h"
#include "feature/feature.h"
#include "base/vcd_dir.h"
#include <string>
//#include "imitation.h"

vcd::FeatureDB *db;
vcd::InfoDB *info_db;

void init() {
    db = new vcd::FeatureDB();
    info_db = new vcd::InfoDB();
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
        impOM->ExtractFrame(buf, w, h, 8);
        impOM->SetKeyId(feature_idx);

        db->AddFeature(impOM);
//        if (++i > 100)
//        break;
    }

    delete [] buf;
}

void t() {
    vcd::FeatureDB f_db;
    vcd::InfoDB i_db;

    f_db.OpenDB("../feature_db");
    i_db.OpenDB("../info/info.db");
//    i_db.print();
}

TEST(speed, extract) {
    init();
    vcd::Dir dir;
    dir.OpenDir("/mnt/share/image_lib");
    std::string subdir;
    while (1) {
        if (dir.GetNextFile(&subdir) == false)
            break;
        
        if (subdir.find('.') != std::string::npos)
            continue;

        printf("%s\n", subdir.c_str());
        process_jpg_dir(subdir.c_str());
    }

    db->Dump("../feature_db");
    info_db->Dump("../info/info.db");
    //process_jpg_dir("/mnt/db/all");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
//    t();
}
