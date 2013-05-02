#include "om_feature.h"
#include "om.h"
#include "common.h"
#include "img_saliency.h"
#include <cv.h>

namespace vcd {

//
// base class [ OM ]
//
OM::OM() {
}

OM::~OM() {
}

bool OM::SetID(uint64 id) {
    _key_id = id;
    return true;
}

uint64 OM::GetID() {
    return _key_id;
}

bool OM::DumpToFile(FILE *pf) {
    fwrite(&_key_id, sizeof(uint64), 1, pf);
    return true;
}

float OM::OMCompare(const uint8 *a, const uint8 *b, const int len_) const {
    int len = len_;

    //
    // array s and d are temp array
    //
    uint8 *s = new uint8[len];
    uint8 *d = new uint8[len];

    // 1==  s[i] = j, if a[j] = i
    for (int i = 0; i < len; ++i) {
        s[a[i]] = i;
    }

    // 2==  d[i] = b[s[i]]
    for (int i = 0; i < len; ++i) {
        d[i] = b[s[i]];
    }

    int max = 0;
    // 3== max(num(d[j] > i), j <= i) (0<=i<len)
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

    delete [] s;
    delete [] d;

    return k;
}

int OM::HammCompare(const uint64 a, const uint64 b) const {
    //
    // get the num of 1 in a^b //
    uint64 c = a ^ b;
    int k = 0;
    while (c != 0) {
        c = c & (c - 1);
        k++;
    }

    return k;
}

/*
 * the len must less than 64
 * the bit at index i is 1 if a[i] > n/2 else is 0
 */
bool OM::ExtractBinaryIndex(const uint8 *a, uint64 *idx, int len) {
    if (len > 64) {
        return 0;
    }

    uint64 seed = 0x1;
    uint8 thres = len / 2;
    uint64 &ret = *idx;
    for (int i = 0; i < len; ++i) {
        if (a[i] < thres) {
            ret = ret | (seed << i);    
        }
    }

    return true;
}

int OM::Compress(const uint8 *f, int len, uint8 *ret) {
    return 0;
}



//
// class of Simply OM
//
SimplyOM::SimplyOM(int n): _binary_idx(0), _n(n) {
    _arr_color = new uint8[n];
}

SimplyOM::~SimplyOM() {
    if (_arr_color != NULL) {
        delete [] _arr_color;
    }
}

float SimplyOM::Compare(const OM *rf_) const {
    const SimplyOM *rf = dynamic_cast<const SimplyOM*>(rf_); 
    // compare the difference of array of sorted color between two feature
    return OMCompare(this->_arr_color, rf->_arr_color, _n);
}

float SimplyOM::SpeedCompare(const OM *rf_, int diff) const {
    const SimplyOM *rf = dynamic_cast<const SimplyOM*>(rf_);

    // speed compare first compare the hamming distance betweent two feature
    if(HammCompare(this->_binary_idx, rf->_binary_idx) > diff) {
        return -1.0;
    }

    return OMCompare(_arr_color, rf->_arr_color, _n);
}

int SimplyOM::GetHashKey(int n) const {
    return 0;
}

bool SimplyOM::DumpToFile(FILE *pf) {
    OM::DumpToFile(pf);
    fwrite(&_n, sizeof(int), 1, pf);
    fwrite(_arr_color, sizeof(uint8), _n, pf);
    fwrite(&_binary_idx, sizeof(uint64), 1, pf);

    return true;
}

SimplyOM* SimplyOM::Extract(const uint8 *data, int w, int h, int n) {
    SimplyOM *feature = new SimplyOM(n);
    get_real_feature(data, w, h, n, feature->_arr_color, NULL);

    return feature;
}

SimplyOM* SimplyOM::ReadFromFile(FILE *pf) {
    // un-beautiful codes
    uint64 key;
    int n;
    fread(&key, sizeof(uint64), 1, pf);
    fread(&n, sizeof(int), 1, pf);

    SimplyOM *feat = new SimplyOM(n);
    feat->SetID(key);
    fread(feat->_arr_color, sizeof(uint8), n, pf);

    return feat;
}

SimplyOM* SimplyOM::SampleFeature() {
    SimplyOM *feat = new SimplyOM(4);
    return feat;
}



//
//
// class of improved OM feature
//
//

ImprovedOM::ImprovedOM(int n): _binary_idx_color(0),
                               _binary_idx_ent(0), _n(n) {
    _arr_color = new uint8[n];
    _arr_entropy = new uint8[n];
}

ImprovedOM::~ImprovedOM() {
    delete [] _arr_color;
    delete [] _arr_entropy;
}

float ImprovedOM::Compare(const OM *rf_) const {
    const ImprovedOM *rf = dynamic_cast<const ImprovedOM *>(rf_);
    float ret_col = OMCompare(this->_arr_color, rf->_arr_color, _n);
    float ret_ent = OMCompare(this->_arr_entropy, rf->_arr_entropy, _n);
    return ret_col > ret_ent ? ret_ent : ret_col;
}

float ImprovedOM::SpeedCompare(const OM *rf_, int diff) const {
    const ImprovedOM *rf = dynamic_cast<const ImprovedOM *>(rf_);
    if (HammCompare(this->_binary_idx_color, 
                      rf->_binary_idx_color) > diff ||
        HammCompare(this->_binary_idx_ent, 
                      rf->_binary_idx_ent) > diff) {
        return -1.0;
    }

    float ret_col = OMCompare(this->_arr_color, rf->_arr_color, _n);
    float ret_ent = OMCompare(this->_arr_entropy, rf->_arr_entropy, _n);
    return ret_col > ret_ent ? ret_ent : ret_col;
}

int ImprovedOM::GetHashKey(int n) const {
    return 0;
}

bool ImprovedOM::DumpToFile(FILE *pf) {
    OM::DumpToFile(pf);
    fwrite(&_n, sizeof(int), 1, pf);
    fwrite(_arr_color, sizeof(uint8), _n, pf);
    fwrite(_arr_entropy, sizeof(uint8), _n, pf);

    return true;
}

ImprovedOM* ImprovedOM::Extract(const uint8 *data, int w, int h, int n) { 
    ImprovedOM *feat = new ImprovedOM(n);
    get_real_feature(data, w, h, n, feat->_arr_color, feat->_arr_entropy);

    return feat;
}

/*
 * (TODO)zhouchao 
 * this method is wrong, if we resize the image to a small size.
 * we get the coordiate of the saliency area is not the same used in
 * the input image.
 * Need a transform !!!!
 *
 */
ImprovedOM* ImprovedOM::ExtractWithSaliency(const uint8 *data, int w, 
                                            int h, int n) {
    cv::Mat img_rgb;
    if (cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return NULL;
    }

    // 1= resize the img to small size in order to reduce
    //    time of get saliency area
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
    ImprovedOM *feat = new ImprovedOM(n);
    get_real_feature(data, w, h, rect, n, feat->_arr_color, feat->_arr_entropy);


    // 5= get the index of feature
//    ExtractIndex(_arr_color, &idx_a);
//    ExtractIndex(_arr_entropy, &idx_b);

    return feat;
}

ImprovedOM* ImprovedOM::ReadFromFile(FILE *pf) {
    // un-beautiful codes
    uint64 key;
    int n;
    fread(&key, sizeof(uint64), 1, pf);
    fread(&n, sizeof(int), 1, pf);

    ImprovedOM *feat = new ImprovedOM(n);
    feat->SetID(key);
    fread(feat->_arr_color, sizeof(uint8), n, pf);
    fread(feat->_arr_entropy, sizeof(uint8), n, pf);

    return feat;
}

} // namespace vcd
