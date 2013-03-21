#include "image_buf.h"
#include "define.h"
#include "imitation.h"
#include "utils.h"
#include "base/logging.h"
#include "gtest/gtest.h"

TEST(ImageBuf, init) {
    vcd::ImageBuf *img_buffer = vcd::image_buffer_init(10, 2, "../feature_db");

    vcd::Imitation img_source;
    img_source.OpenJpgDb("../img/");
    uint8 data[1024 * 900];
    int w, h;
    uint64 feat = 1;
    while (img_source.GetNextImg(data, 1024 * 900, &w, &h)) {
        img_buffer->AppendImage(data, w, h, feat++);
        img_buffer->DumpInfo();
    }
}

//TEST(ImageBuf, bigdata) {
//    vcd::ImageBuf *img_buffer = vcd::image_buffer_init(20, 1, "/mnt/share/test_db/");
//
//    vcd::Imitation img_source;
//    img_source.OpenJpgDb("/mnt/db/1/");
//    uint8 data[1024 * 900 * 3];
//    int w, h;
//    uint64 feat = 1;
//    while (img_source.GetNextImg(data, 1024 * 900 * 3, &w, &h)) {
//        vcd::insert_repeat_feature(feat);
//        img_buffer->AppendImage(data, w, h, feat++);
//        //img_buffer->DumpInfo();
//    }
//}

int yuv_process(uint8 *data, int w, int h) {
//    show_yuv_colorful(data, w, h);
    return 1;
}

TEST(Imitation, single_video_reader) {
    vcd::Imitation::ProcessVideo("/mnt/share/libVideos/327681.mp4", yuv_process);
}

TEST(Imitation, video_reader) {
    vcd::Imitation source;
    source.OpenJpgDb("/mnt/share/libVideos");
    source.GetVideo(yuv_process);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
