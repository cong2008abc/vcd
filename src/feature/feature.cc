#include "feature.h"
#include "om.h"
#include "common.h"
#include "img_saliency.h"
#include <stdlib.h>
#include <math.h>
#include <cv.h>

namespace vcd {

/*
 * implement get/set of key_id
 */
int Feature::GetKeyId() const {
    return _feature_id;
}

int Feature::SetKeyId(int key_id) {
    _feature_id = key_id;
    return 1;
}

float Feature::Compare(const Feature *a, float f) {}



/*
 * implement ImproveOMFeature class
 */
int ImpOMFeature::LEN = 64;

ImpOMFeature::ImpOMFeature() {
    _arr_color = NULL;
    _arr_entropy = NULL;
}

ImpOMFeature::~ImpOMFeature() {
}

/*
 * this is a test extract function.
 * extract a 4 * 4 OM feature from the whole image
 * remain it is test the discriminate of OM feature
 */
bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h) {
    int n = 8;
    _arr_color = new uint8[n * n];
    _arr_entropy = new uint8[n * n];

    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);
    for (int i = 0; i < n * n; ++i) _arr_entropy[i] = 0;

    ExtractIndex(_arr_color, &idx_a);
    ExtractIndex(_arr_entropy, &idx_b);

    return true;
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h, int n) {
    _arr_color = new uint8[n * n];
    _arr_entropy = new uint8[n * n];
    
    // 1= cvt the yuv data to rgb
    cv::Mat img_rgb;
    if (cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return false;
    }

    resize_mat_by_width(img_rgb, img_rgb, 160);

    cv::Mat img = img_rgb;
    cv::Rect margin;
    remove_margin(img, &margin);

    // 2= extract the main rectangle on rgb image
    cv::Mat roi(img, margin);
    cv::Rect rect;
    cv::Mat saliency_map;
    Saliency::Get(roi, saliency_map);    
    Saliency::ExtractView(saliency_map, rect);

    // 3= convert to the whole coordiate
    rect.x += margin.x;
    rect.y += margin.y;

    // 4= extract om feature on yuv data
    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);


    // 5= get the index of feature
    ExtractIndex(_arr_color, &idx_a);
    ExtractIndex(_arr_entropy, &idx_b);

    return true;
}

bool ImpOMFeature::DumpToFile(FILE *pfile) {
    /*
     * here we test the feature effective when feature_len = 16
     * so the val of the element in om_feature_array is between 0~16
     * if the element is compressed to 4bit(2^4 = 64).
     * then the whole om_feature can be compressed to 4*N/8=8B (N=16).
     */
    if (ImpOMFeature::LEN == 16) {
        int size_of_compress = ImpOMFeature::LEN * 4 / 8;
        uint8 *compress = new uint8[size_of_compress];
        if (_arr_color != NULL) {
            CompressFeature(_arr_color, compress, ImpOMFeature::LEN);
            fwrite(compress, sizeof(uint8), ImpOMFeature::LEN, pfile);
        }
        if (_arr_entropy != NULL) {
            CompressFeature(_arr_entropy, compress, ImpOMFeature::LEN);
            fwrite(compress, sizeof(uint8), ImpOMFeature::LEN, pfile);
        }
        delete [] compress;
    } else {
        /*
         * otherwise don't compress the feature
         */
        fwrite(_arr_color, sizeof(uint8), ImpOMFeature::LEN, pfile);
        fwrite(_arr_entropy, sizeof(uint8), ImpOMFeature::LEN, pfile);
    }            
    return true;
}

bool ImpOMFeature::ReadFromFile(FILE *pfile) {
    _arr_color = new uint8[ImpOMFeature::LEN];
    _arr_entropy = new uint8[ImpOMFeature::LEN];

    fread(_arr_color, sizeof(uint8), ImpOMFeature::LEN, pfile);
    fread(_arr_entropy, sizeof(uint8), ImpOMFeature::LEN, pfile);

    ExtractIndex(_arr_color, &idx_a);
    ExtractIndex(_arr_entropy, &idx_b);

    return true;
}

int get_diff(uint64 a, uint64 b) {
    uint64 c = a ^ b;
    int k = 0;
    while (c != 0) {
        c = c & (c - 1);
        k++;
    }

    return k;
}

float ImpOMFeature::Compare(const Feature *rf, float thres) {
    const ImpOMFeature *_rf = dynamic_cast<const ImpOMFeature*>(rf);

    int diff_thre = 22;
    if (get_diff(idx_a, _rf->idx_a) > diff_thre ||
        get_diff(idx_b, _rf->idx_b) > diff_thre)
        return -1.0;

    float ret_col = InterCompare(_arr_color, _rf->_arr_color);
    if (ret_col < thres)
        return ret_col;

    float ret_ent = InterCompare(_arr_entropy, _rf->_arr_entropy);

    return ret_col > ret_ent ? ret_ent : ret_col;
}

float ImpOMFeature::Compare(const Feature *rf) {
    //TODO(zhouchao) compare two feature distance
    //...
    const ImpOMFeature *real_rf = dynamic_cast<const ImpOMFeature*>(rf);
    return Compare(real_rf);
}

float ImpOMFeature::Compare(const ImpOMFeature *rf) {
    float ret_color = InterCompare(_arr_color, rf->_arr_color);
    float ret_entropy = InterCompare(_arr_entropy, rf->_arr_entropy);
    //printf("entropy %f\n", ret_entropy);
    return ret_color > ret_entropy ? ret_entropy : ret_color;
}

bool ImpOMFeature::GetCompressFeature(uint8 *result) const {
    return CompressFeature(_arr_color, result, ImpOMFeature::LEN);
}

/*
 * this is compare method may be wrong!
 */
float ImpOMFeature::InterCompare(const uint8 *arr_a, const uint8 *arr_b) {
    uint8 s[ImpOMFeature::LEN], d[ImpOMFeature::LEN];
    int len = ImpOMFeature::LEN;
    for (int i = 0; i < len; ++i) {
        s[arr_a[i]] = i;
    }

    for (int i = 0; i < len; ++i) {
        d[i] = arr_b[s[i]];
    }

    int max = 0;
    for (int i = 0; i < len; ++i) {
        int tmp = i;
        for (int j  = 0; j <= i; j++) {
            if (d[j] <= i) tmp--;
        } 
        if (tmp > max) {
            max = tmp;
        }
    }

    int n = len / 2;
    float k = (n - 2 * max) / (float)n;

    return k;
}

bool ImpOMFeature::ExtractIndex(const uint8 *data, uint64 *idx_a) {
    uint64 seed = 0x1;
    unsigned char thres = ImpOMFeature::LEN / 2;
    uint64 &ret = *idx_a;
    for (int i = 0; i < ImpOMFeature::LEN; ++i) {
        if (data[i] < thres) {
            ret = ret | (seed << i);    
        }
    }

    return true;
}

/*
 * this compress function try to compress the 16*8B to 8B
 */
bool ImpOMFeature::CompressFeature(const uint8 *data, uint8 *result,int n) const {
    uint64 *pres = reinterpret_cast<uint64*>(result);
    uint8 bit = 4;
    uint8 mask = (1 << bit) - 1;
    uint64 &rres = *pres;
    for (int i = 0; i < n; ++i) {
        rres = (rres << bit) | (data[i] & mask);
    }
    return true;
}

void ImpOMFeature::print() const {
    if (_arr_color != NULL) {
        printf("color array: ");
        for (int i = 0; i < ImpOMFeature::LEN; ++i) {
            printf("%d ", _arr_color[i]);
        }
        printf("\n");
    }
    if (_arr_entropy != NULL) {
        printf("entropy array: ");
        for (int i = 0; i < ImpOMFeature::LEN; ++i) {
            printf("%d ", _arr_entropy[i]);
        }
        printf("\n");
    }
}

/*
 * SimplyOMFeature
 *
 */
SimplyOMFeature::SimplyOMFeature() {
}

SimplyOMFeature::~SimplyOMFeature() {
}

bool SimplyOMFeature::ExtractFrame(const uint8 *data, int w, int h, int n) {
    if (data == NULL|| n <= 0) {
        return false;
    }
//    _arr_color = new uint8[n * n];
//
//    get_real_feature(data, w, h, n, _arr_color, NULL);
//
//    ExtractIndex(_arr_color, &idx_a);

    return true;
}

float SimplyOMFeature::Compare(SimplyOMFeature *rf, float thres) {
//    const SimplyOMFeature* _rf = rf;
    return -10.0;

//    int diff_thre = 22;
//    if (get_diff(idx_a, _rf->idx_a) > diff_thre ||
//        get_diff(idx_b, _rf->idx_b) > diff_thre)
//        return -1.0;
//
//    float ret_col = InterCompare(_arr_color, _rf->_arr_color);
//    if (ret_col < thres)
//        return ret_col;
//
//    float ret_ent = InterCompare(_arr_entropy, _rf->_arr_entropy);
//
//    return ret_col > ret_ent ? ret_ent : ret_col;
}

/*
 * saliency OM feature class
 *
 */

SaliencyOMFeature::SaliencyOMFeature() {
}

SaliencyOMFeature::~SaliencyOMFeature() {
}

bool SaliencyOMFeature::ExtractFrame(const uint8 *data, int w, int h, int n) {
    if (data == NULL) {
        return false;
    }

//    if (_arr_color != null) {
//        delete [] _arr_color;
//    }
//    if (_arr_entropy != null) {
//        delete [] _arr_entropy;
//    }
//
//    _arr_color = new uint8[n * n];
//    _arr_entropy = new uint8[n * n];
//
//    // 1= cvt the yuv data to rgb
//    cv::Mat img_rgb;
//    if (cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
//        return false;
//    }
//
//    resize_mat_by_width(img_rgb, img_rgb, 160);
//
//    cv::Mat img = img_rgb;
//    cv::Rect margin;
//    remove_margin(img, &margin);
//
//    // 2= extract the main rectangle on rgb image
//    cv::Mat roi(img, margin);
//    cv::Rect rect;
//    cv::Mat saliency_map;
//    Saliency::Get(roi, saliency_map);    
//    Saliency::ExtractView(saliency_map, rect);
//
//    // 3= convert to the whole coordiate
//    rect.x += margin.x;
//    rect.y += margin.y;
//
//    // 4= extract om feature on yuv data
//    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);
//
//
//    // 5= get the index of feature
//    ExtractIndex(_arr_color, &idx_a);
//    ExtractIndex(_arr_entropy, &idx_b);

    return true;
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
