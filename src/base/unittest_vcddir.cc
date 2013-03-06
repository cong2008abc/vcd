#include "base/logging.h"
#include "gtest/gtest.h"
#include "vcd_dir.h"

TEST(vcd_dir, open) {
    vcd::Dir dir; 
    dir.OpenDir(".");
}

TEST(vcd_dir, readnext) {
    vcd::Dir dir(".");
    std::string file;
    while (dir.GetNextFile(&file)) {
        VLOG(1, "%s", file.c_str());
    }
}

TEST(vcd_dir, getall) {
    vcd::Dir dir("..");
    std::vector<std::string> list;
    if (dir.GetAllFile(&list)) {
        for (size_t t = 0; t < list.size(); ++t) {
            VLOG(1, "%s", list[t].c_str());
        }
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
