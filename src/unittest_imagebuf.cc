#include "define.h"
#include "imitation.h"
#include "image_buf.h"
#include "base/logging.h"
#include "gtest/gtest.h"

TEST(ImageBuf, init) {
    vcd::ImageBuf *img_buffer = vcd::image_buffer_init(10, 2, "../feature/db");

    vcd::Imitation img_source;
    img_source.OpenJpgDb("../img/");
    uint8 data[1024 * 900];
    int w, h;
    uint64 feat = 1;
    while (img_source.GetNextImg(data, 1024 * 900, &w, &h)) {
        img_buffer->AppendImage(data, w, h, feat++);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
