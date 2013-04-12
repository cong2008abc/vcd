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
    int n = 6;
    _arr_color = new uint8[n * n];
    _arr_entropy = new uint8[n * n];

    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);
    //for (int i = 0; i < n * n; ++i) _arr_entropy[i] = 0;
    return true;
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h, int n) {
    _arr_color = new uint8[n * n];
    _arr_entropy = new uint8[n * n];
    
    // 1= cvt the yuv data to rgb
    //IplImage *img_rgb = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
    cv::Mat img_rgb;
    if (cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return false;
    }

    resize_mat_by_width(img_rgb, img_rgb, 160);

    cv::Mat img = img_rgb;
    cv::Rect margin;
    remove_margin(img, &margin);
//
//    // 2= extract the main rectangle on rgb image
    cv::Mat roi(img, margin);
//    cv::Mat roi = img_rgb;
    cv::Rect rect;
    cv::Mat saliency_map;
    Saliency::Get(roi, saliency_map);    
    Saliency::ExtractView(saliency_map, rect);

    // 3= convert to the whole coordiate
    rect.x += margin.x;
    rect.y += margin.y;

//    show_mat(saliency_map);

    // 3= extract om feature on yuv data
    get_real_feature(data, w, h, n, _arr_color, _arr_entropy);
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
    return true;
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

bool ImpOMFeature::ExtractIndex(const uint8 *data, int *idx_a, int *idx_b) {
    //
    return false;
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
