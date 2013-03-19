#include "define.h"
#include "om_interface.h"
#include "imitation.h"
#include "image_buf.h"
#include "gtest/gtest.h"

char db_path[] = "../feature_db";

TEST(om_interface, open) {
    open_db(db_path);
}

TEST(om_interface, query) {
    vcd::Imitation img_source;  
    img_source.OpenJpgDb("/mnt/db/1");
    unsigned char data[1024 * 900 * 3];
    int w, h;

    while (img_source.GetNextImg(data, 1024 * 900 * 3, &w, &h)) {
        query_image(data, w, h);
    }

//    close_db();
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
