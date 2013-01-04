#include "feature.h"
#include "om.h"
#include <stdlib.h>
#include <math.h>

namespace vcd {

/*
 * implement get/set of key_id
 */
int Feature::GetKeyId() {
    return _feature_id;
}

int Feature::SetKeyId(int key_id) {
    _feature_id = key_id;
    return 1;
}



/*
 * implement ImproveOMFeature class
 */
int ImpOMFeature::FEATURE_LEN = 16;

ImpOMFeature::ImpOMFeature() {
    _arr_color = NULL;
    _arr_entropy = NULL;
}

ImpOMFeature::~ImpOMFeature() {
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h) {
    _arr_color = new uint8[ImpOMFeature::FEATURE_LEN];
    //_arr_entropy = new uint8[ImpOMFeature::FEATURE_LEN];
    
    int n = static_cast<int>(sqrt(ImpOMFeature::FEATURE_LEN));

    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);

    return true;
}

bool ImpOMFeature::DumpToFile(FILE *pfile) {
    fwrite(_arr_color, sizeof(uint8), ImpOMFeature::FEATURE_LEN, pfile);
    fwrite(_arr_entropy, sizeof(uint8), ImpOMFeature::FEATURE_LEN, pfile);
    return true;
}

bool ImpOMFeature::ReadFromFile(FILE *pfile) {
    _arr_color = new uint8[ImpOMFeature::FEATURE_LEN];
    _arr_entropy = new uint8[ImpOMFeature::FEATURE_LEN];

    fread(_arr_color, sizeof(uint8), ImpOMFeature::FEATURE_LEN, pfile);
    fread(_arr_entropy, sizeof(uint8), ImpOMFeature::FEATURE_LEN, pfile);
    return true;
}

float ImpOMFeature::Compare(const Feature *rf) {
    //TODO(zhouchao) compare two feature distance
    //...

    const ImpOMFeature *ptr_r = dynamic_cast<const ImpOMFeature*>(rf);
    float ret_color = InterCompare(_arr_color, ptr_r->_arr_color);
    float ret_entropy = InterCompare(_arr_entropy, ptr_r->_arr_entropy);

    return ret_color > ret_entropy ? ret_entropy : ret_color;
}

float ImpOMFeature::InterCompare(const uint8 *arr_a, const uint8 *arr_b) {
    uint8 s[ImpOMFeature::FEATURE_LEN], d[ImpOMFeature::FEATURE_LEN];
    int len = ImpOMFeature::FEATURE_LEN;
    for (int i = 0; i < len; ++i) {
        s[arr_a[i]] = i;
    }

    for (int i = 0; i < len; ++i) {
        d[i] = arr_b[s[i]];
    }

    int max = 0;
    for (int i = 0; i < len; ++i) {
        int tmp = 0;
        for (int j  = 0; j <= i; j++) {
            if (s[j] > i) tmp++;
        }
        if (tmp > max) {
            max = tmp;
        }
    }

    int n = len / 2;
    float k = (n - 2 * max) / (float)n;

    return k;
}

bool ImpOMFeature::ExtractIndex(const uint8 *data, int *idx_a, int *idx_b) {
    //
}



/*
 * non-member functions
 */
Feature *FeatureFactory(int type) {
    switch (type) {
        case kImprovedOM:
            return new ImpOMFeature();
    }
    return NULL;
}

} // namespace vcd
