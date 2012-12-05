#include "feature.h"
#include "om.h"
#include <stdlib.h>

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
int ImpOMFeature::FEATURE_LEN = 64;

ImpOMFeature::ImpOMFeature() {
    _arr_color = NULL;
    _arr_entropy = NULL;
}

ImpOMFeature::~ImpOMFeature() {
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h) {
    _arr_color = new uint8[ImpOMFeature::FEATURE_LEN];
    _arr_entropy = new uint8[ImpOMFeature::FEATURE_LEN];

    get_real_feature(data, w, h, 8, _arr_color, _arr_entropy);

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

float ImpOMFeature::Compare(const Feature &rf) {
    //TODO(zhouchao) compare two feature distance
    //...

    return 1.0;
}

Feature *FeatureFactory(int type) {
    switch (type) {
        case kImprovedOM:
            return new ImpOMFeature();
    }
    return NULL;
}

} // namespace vcd
