#include "base/logging.h"
#include "gtest/gtest.h"
#include "om_feature.h"
#include "om_index.h"
#include "om.h"
#include <cv.h>
#include <highgui.h>

//const char *path[] = {"../../img/5.jpg",
//                      "../../img/6.jpg",
//                      "../../img/7.jpg"
//                     };

vcd::OMIndex feature_db(8177);

TEST(om_index, insert) {
    int kTestNum = 1000 * 1000 * 10;
    while (kTestNum--) {
        vcd::OM *feat = vcd::SimplyOM::SampleFeature(64);
        //feat->Print();
        feature_db.Insert(feat); 
    }
    feature_db.PrintCurrentInfo();
}

TEST(om_index, query) {
    int kQuery = 1000 * 1;
    while (kQuery--) {
        vcd::OM *feat = vcd::SimplyOM::SampleFeature(64);
        const vcd::OM *ret;
        feature_db.Query(feat, &ret);
        //printf("%f\n", feature_db.Query(feat, &ret));
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv); 

    return RUN_ALL_TESTS();
}
