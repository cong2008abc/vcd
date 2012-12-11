#include "utils.h"
#include <stdio.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Wrong Parameter Number!\n");
        exit(1);
    }

    show_yuv(argv[1]);
}
