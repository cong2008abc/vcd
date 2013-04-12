#include "imitation.h"
#include "utils.h"
#include "simply_feature_index.h"
#include "base/logging.h"
#include "base/vcd_dir.h"
#include "feature/common.h"
#include "gtest/gtest.h"
#include <cv.h>
#include <highgui.h>


int idx = 0;
vcd::uint64 pre = 0;
vcd::SimplyIndex *sim_idx;

int show_yuv_colorful_unconst(unsigned char *data, int w, int h) {
    //printf("%d\n", idx++);
    //return 1;
    cv::Mat img_rgb;
    if (vcd::cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return 1;
    }

    resize_mat_by_width(img_rgb, img_rgb, 320);
    vcd::uint64 cur = vcd::get_simply_feature(img_rgb);

    //printf("%llu\n", cur);

    int res = sim_idx->Insert(cur);
    if (res < 0) {
        //show_mat(img_rgb, 20);
        //show_mat(img_rgb);

        //char sname[128];
        //sprintf(sname, "/mnt/share/image_db/%d.jpg", -1 * res);
        //show_image_from_path(sname);
        //cv::Mat match = cv::imread(sname);
        //show_mat(match);

        return 1;
    }

    char name[127];
    sprintf(name, "/mnt/share/image_db/%d.jpg", res + idx);
    cv::imwrite(name, img_rgb);

    return 0;
}

void test_dir2(const char *path) {
    vcd::Dir dir;
    std::string file_name;
    if (dir.OpenDir(path) == false)
        return;
    
    int k = 0;
    while (1) {
        if (dir.GetNextFile(&file_name) == false)
            break;

        printf("%s\n", file_name.c_str());
        if (vcd::Imitation::ProcessVideo(file_name.c_str(),
                                         show_yuv_colorful_unconst) == false) {
            //printf("ok\n");
            continue;
        }
        //if (k++ > 3)
        //    break;
    }
}

const char *prefix = "/mnt/book/VideoData/part";

TEST(sample, video) {
    char video_dir[128];
    int num = 100;
    for (int i = 369; i < 369 + 100; ++i) {
        sprintf(video_dir, "%s%d", prefix, i);
        sim_idx->Clear();
        idx = i * 100000;
        test_dir2(video_dir);
    }
}

void init() {
    sim_idx = new vcd::SimplyIndex();
}

int main(int argc, char **argv) {
    init();

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
