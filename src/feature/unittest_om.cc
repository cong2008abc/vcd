#include "base/logging.h"
#include "gtest/gtest.h"
#include "../utils.h"
#include "om.h"
#include <cv.h>
#include <highgui.h>

const char *path[] = {"../../img/5.jpg",
                      "../../img/6.jpg",
                      "../../img/7.jpg"
                     };

void test_remove_margin(const char *_p) {
    cv::Mat mat = cv::imread(_p); 
    EXPECT_TRUE(mat.data);

    cv::Rect rect;  
    remove_margin(mat, &rect);

    //mat.row(rect.y) = cv::Scalar(255);
    cv::rectangle(mat, rect, cv::Scalar(255));

    show_mat(mat);
}

TEST(om, remove_margin) {
    for (int i = 0; i < 3; ++i) {
        test_remove_margin(path[i]);
        printf("\n");
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv); 

    return RUN_ALL_TESTS();
}
