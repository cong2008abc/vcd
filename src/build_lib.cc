#include "data_stream.h"

const int LIB_NUM = 3;
const char *lib[] = {"/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};

int main() {
    vcd::DataStream ds;

    for (int i = 0; i < LIB_NUM; ++i) {
        ds.BuildMediaLib(lib[i]);
    }
}
