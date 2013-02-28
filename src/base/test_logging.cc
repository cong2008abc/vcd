#include "base/logging.h"
#include "gtest/gtest.h"

TEST(logging, print) {
    VLOG(0, "%s", "stupid person!\n");
    VLOG(0, "%d\n", 2);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
