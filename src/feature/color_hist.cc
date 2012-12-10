#include "color_hist.h"

namespace vcd {

const int HIST_NUM = 16;
const int N = 3;
int ColorHistFeature::FEATURE_LEN = N * N * HIST_NUM;

ColorHistFeature::ColorHistFeature() {
    _arr_hist = NULL;
}

ColorHistFeature::~ColorHistFeature() {
    if (_arr_hist != NULL) {
        delete [] _arr_hist;
    }
}

bool ColorHistFeature::ExtractFrame(const uint8 *data, int w, int h) {
    _arr_hist = new float[ColorHistFeature::FEATURE_LEN];

    int count[HIST_NUM];
    int max_val = 256;

    int sub_w = w / N;
    int sub_h = h / N;
    int total = sub_w * sub_h;

    int idx = 0;

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < HIST_NUM; ++k) {
                count[k] = 0;
            }

            for (int y = i * sub_h; y < (i + 1) * sub_h; ++y) {
                for (int x = j * sub_w; x < (j + 1) * sub_w; ++x) {
                    int val = data[y * h + x];
                    count[val * HIST_NUM / 256]++;
                }
            }

            for (int k = 0; k < HIST_NUM; ++k) {
                _arr_hist[idx++] = count[k] / (float)total;
            }
        }
    }

    return true;
}

bool ColorHistFeature::DumpToFile(FILE *pf) {}

bool ColorHistFeature::ReadFromFile(FILE *pf) {}

float ColorHistFeature::Compare(const Feature *rf) {
    const ColorHistFeature *ptr = dynamic_cast<const ColorHistFeature*>(rf);
    float ret = 0.0f;
    for (int i = 0; i < ColorHistFeature::FEATURE_LEN; ++i) {
        ret += ABS(_arr_hist[i] - ptr->_arr_hist[i]);
    }

    return ret;
}

float ColorHistFeature::ABS(float x) {
    return x > 0 ? x : -x;
}

} // namespace vcd
