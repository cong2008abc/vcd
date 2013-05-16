#include "utils.h"
#include "global.h"
#include "feature/om_feature.h"

const int kbuf_size = 1024 * 1024 * 2;
vcd::uint8 buf[kbuf_size];

vcd::OM* get_feature(const char *jpg) {
    int w, h;
    if (simulate_input(jpg, buf, kbuf_size, &w, &h) == false) {
        return NULL;
    }

    vcd::OM *ret;
    switch (Global::query_method) {
        case FEAT_OM:
            ret = vcd::SimplyOM::Extract(buf, w, h, Global::sub_num);
            break;
        case FEAT_IMP_OM:
            ret = vcd::ImprovedOM::Extract(buf, w, h, Global::sub_num);
            break;
        case FEAT_SA_OM:
            ret = vcd::ImprovedOM::ExtractWithSaliency(buf, w, h, Global::sub_num);
            break;
        default:
            return NULL;
    }

    return ret;
}

void compare_two(const char *a, const char *b) {
    vcd::OM *fa = get_feature(a);
    vcd::OM *fb = get_feature(b);

    printf("distance: %s %s\n", a, b);
    if (fa == NULL || fb == NULL) {
        printf("some image load error!\n");
    } else {
        printf("%f\n", fa->Compare(fb));
    }
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Parameter!\n");
        return 1;
    }

    compare_two(argv[1], argv[2]);
}
