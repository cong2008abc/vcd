#include "om_feature.h"
#include "om.h"
#include "common.h"
#include "img_saliency.h"
#include <cv.h>
#include <stdlib.h>
#include <time.h>

namespace vcd {

static const int pattern_size = 7;
static const uint8 pattern_seed[] = {2, 8, 17, 51, 22, 37, 48,
                   10, 19, 32, 43, 54, 25, 18,
                   20, 25, 12, 27, 29, 35, 46,
                   26, 37, 49, 59, 16, 27, 18,
                   29, 31, 33, 35, 40, 41, 42,
                   30, 31, 32, 33, 34, 35, 36,
                   36, 37, 38, 39, 40, 41, 42,
                   40, 42, 44, 46, 48, 49, 50,
                   50, 51, 52, 53, 54, 55, 56,
                   58, 59, 60, 61, 62, 63, 0};
//static const uint8 pattern_seed[] = {2, 3, 4, 5, 6, 7, 8,
//                   9, 10, 11, 12, 13, 14, 15,
//                   16, 17, 18, 19, 20, 21, 22,
//                   23, 24, 25, 26, 27, 28, 29,
//                   20, 21, 22, 23, 24, 25, 26,
//                   30, 31, 32, 33, 34, 35, 36,
//                   36, 37, 38, 39, 40, 41, 42,
//                   40, 42, 44, 46, 48, 49, 50,
//                   50, 51, 52, 53, 54, 55, 56,
//                   58, 59, 60, 61, 62, 63, 0};

void print_bits(uint64 bits, int n = 64) {
    uint64 t = bits;
    for (int i = 0; i < n; i++) {
        if (t & (1 << (n - i - 1))) {
            printf("1");
        } else {
            printf("0");
        }
        //t >>= 1;
    }
    printf("\n");
}

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

uint64 OM::GetID() const {
    return _key_id;
}

bool OM::DumpToFile(FILE *pf) {
    fwrite(&_key_id, sizeof(uint64), 1, pf);
    return true;
}

bool OM::Print() {
    printf("%llu:", _key_id);
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

    //printf("%d\n", k);

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
//    for (int i = 0; i < len; ++i) {
//        ret <<= 1;
//        if (a[i] < thres) {
//            //ret = ret | (seed << i);    
//            ret = ret | seed;
//        }
//    }

    ret = 0;
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            ret <<= 1;
            if (a[j * 8 + i] < thres) {
                ret = ret | seed;
            }
        }
    }

    return true;
}

int OM::Compress(const uint8 *f, int len, uint8 *ret) {
    return 0;
}

struct sort_t {
    int pre;
    int idx;
};

bool sort_t_cmp(const sort_t &a, const sort_t &b) {
    return a.pre < b.pre;
}

uint64 OM::HashArray(const uint8 *a, int len, int n) const {
    const int interval = 2; 
    sort_t *temp = new sort_t[n];
    //const int pos[] = {19, 22, 27, 29, 32, 31};
    //const int pos[] = {19, 20, 21, 22, 23, 24};
    const int pos[] = {5, 7, 9, 10, 12, 14};
    for (int i = 0; i < n; i++) {
        temp[i].pre = a[pos[i]];
        temp[i].idx = i;
    }

    std::sort(temp, temp + n, sort_t_cmp);

    static int number[] = {1, 2, 6, 24, 120, 720, 5040};
    uint64 ret = 0;
    uint8 factor;
    for (int i = 0; i < n; i++) {
        factor = temp[i].idx;
        for (int j = i - 1; j >= 0; j--) {
            if (temp[j].idx < temp[i].idx)
                factor--;
        } 
        ret += factor * number[n - 1 - i];
    }
    fprintf(stderr, "??%d\n", ret);

    delete [] temp;

    return ret;
}

uint64 OM::HashArray(const uint8 *a, const uint8 *choose, int n) const {
    sort_t *temp = new sort_t[n];
    const uint8 *pos = choose;
    for (int i = 0; i < n; i++) {
        temp[i].pre = a[pos[i]];
        temp[i].idx = i;
    }

    std::sort(temp, temp + n, sort_t_cmp);
    static int number[] = {1, 2, 6, 24, 120, 720, 5040};
    uint64 ret = 0;
    uint8 factor;
    
//    printf("?? subrange %d:", n);

    for (int i = 0; i < n - 1; i++) {
//        printf("%d ", temp[i].idx);
        factor = temp[i].idx;
        for (int j = i - 1; j >= 0; j--) {
            if (temp[j].idx < temp[i].idx)
                factor--;
        } 
        ret += factor * number[n - 2 - i];
    }

//    printf("\n");

    delete [] temp;

    return ret;
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

uint64 SimplyOM::GetHashKey(int n) const {
    return HashArray(_arr_color, _n, n);
    //return 0;
}

bool SimplyOM::GetHashKeys(int n, uint64 *ret, int num) const {
    static const uint8 *pattern = pattern_seed;

    for (int i = 0; i < 4; i++) {
        ret[i] = HashArray(_arr_color, pattern + i * pattern_size, n); 
    }
    for (int i = 0; i < num; i++) {
        printf("?>> %d\n", ret[i]);
        //ret[i] = ret[i] * ret[3 - i];
    }

    return true;
    //return false;
}

bool SimplyOM::DumpToFile(FILE *pf) {
    OM::DumpToFile(pf);
//    fprintf(stderr, "%d\n", _n);
    fwrite(&_n, sizeof(int), 1, pf);
    fwrite(_arr_color, sizeof(uint8), _n, pf);
    fwrite(&_binary_idx, sizeof(uint64), 1, pf);

    return true;
}

bool SimplyOM::Print() {
    OM::Print();
    printf("\t");
    for (int i = 0; i < _n; i++) {
        printf("%d|", _arr_color[i]);
    }
    printf("\n");
    return true;
}

SimplyOM* SimplyOM::Extract(const uint8 *data, int w, int h, int n) {
    SimplyOM *feature = new SimplyOM(n * n);
    get_real_feature(data, w, h, n, feature->_arr_color, NULL);

    return feature;
}

SimplyOM* SimplyOM::ReadFromFile(FILE *pf) {
    // un-beautiful codes
    uint64 key;
    int n;
    int ret = fread(&key, sizeof(uint64), 1, pf);
    if (ret != 1) {
        return NULL;
    }
    if (fread(&n, sizeof(int), 1, pf) != 1) {
        return NULL;
    }

 //   fprintf(stderr, "%llu %d\n", key, n);

    SimplyOM *feat = new SimplyOM(n);
    feat->SetID(key);
    ret = fread(feat->_arr_color, sizeof(uint8), n, pf);
    if (ret != n) {
        goto Failure;
    }
    ret = fread(&feat->_binary_idx, sizeof(uint64), 1, pf);
    if (ret != 1) {
        goto Failure;
    }

    feat->ExtractBinaryIndex(feat->_arr_color, &feat->_binary_idx, n);

    feat->Print();

    return feat;
Failure:
    delete feat;
    return NULL;
}

SimplyOM* SimplyOM::SampleFeature(int n) {
    SimplyOM *feat = new SimplyOM(n);

    for (int i = 0; i < n; i++) {
        feat->_arr_color[i] = i;
    }

    //
    // get a rand feat
    //
//    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        int k = rand() % (n - i);

        int tmp = feat->_arr_color[k];
        feat->_arr_color[k] = feat->_arr_color[n - 1 - i];
        feat->_arr_color[n - 1 - i] = tmp;
    }

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

uint64 ImprovedOM::GetHashKey(int n) const {
    return HashArray(_arr_color, _n, n);
}

bool ImprovedOM::GetHashKeys(int n, uint64 *ret, int num) const {
    static const uint8 *pattern = pattern_seed;

    for (int i = 0; i < num; i++) {
        ret[i] = HashArray(_arr_color, pattern + i * pattern_size, n); 
    }
//    for (int i = 0; i < num; i++) {
////        printf("?? ret val: %d\n", ret[i]);
//        ret[i] = ret[i];
//    }

    return true;
}

uint64 get_sub_bits(uint64 bits,int len, int start, int n) {
    int mask = (1 << n) - 1; 
    int end = start + n;
    int remove = len - end;
    
    return (bits >> remove) & mask;
}

//
// new hash function by using index_feature
//
//bool ImprovedOM::GetHashKeys(int n, uint64 *ret, int num) const {
//    int mask_len = (n + 2) / 3;
//    //int mask = (1 << mask_len) - 1;
//    static const uint8 pattern[] = {1, 6, 12, 14, 18, 20, 24, 26, 28, 30, 36, 42, 48, 50};
//    static const uint8 pattern_m[] = {3, 8, 10, 11, 16, 22, 25, 27, 31, 33, 35, 39, 43, 45};
//    static const uint8 pattern_l[] = {52, 48, 45, 42, 40, 37, 35, 32, 30, 28};
//    int start_low = 0, start_up = 8;
//    int low_p = start_low;
//    int up_p = start_up;
//    const int interval = 6;
//
//    for (int i = 0; i < num; i++) {
////        int end = pattern[i] + n;
////        int remove = _n - end;
////        ret[i] = (_binary_idx_color >> remove) & mask;
//        //ret[i] = _binary_idx_color;
////        ret[i] = (get_sub_bits(_binary_idx_color, _n, 
////                               pattern[up_p], mask_len) << mask_len) | 
////                 (get_sub_bits(_binary_idx_color, _n,
////                               pattern[low_p], mask_len));
////        uint64 p1 = get_sub_bits(_binary_idx_color, _n,
////                                 pattern[i], mask_len);
////        uint64 p3 = get_sub_bits(_binary_idx_color, _n,
////                                 pattern_m[i], mask_len);
////        uint64 p2 = get_sub_bits(_binary_idx_color, _n,
////                                 pattern_l[i], mask_len);
////        
////        ret[i] = (p2 << (mask_len * 2)) | (p3 << (mask_len)) | p1;
//        //ret[i] = _binary_idx_color;
//
//        ret[i] = get_sub_bits(_binary_idx_color, _n, i * interval, n);
//
//        //printf("|| %llu %llu %llu %llu\n", ret[i]);
////        printf("|| %llu ", ret[i]);
////        print_bits(ret[i], n);
//    }
//
//    return true;
//}

bool ImprovedOM::DumpToFile(FILE *pf) {
    OM::DumpToFile(pf);
    fwrite(&_n, sizeof(int), 1, pf);
    fwrite(_arr_color, sizeof(uint8), _n, pf);
    fwrite(_arr_entropy, sizeof(uint8), _n, pf);

    return true;
}

bool ImprovedOM::Print() {
    OM::Print();
    printf("\t");
    for (int i = 0; i < _n; i++) {
        printf("%d|", _arr_color[i]);
    }
    printf("\n\t");
    for (int i = 0; i < _n; i++) {
        printf("%d|", _arr_entropy[i]);
    }
    printf("\n");
    return true;
}

ImprovedOM* ImprovedOM::Extract(const uint8 *data, int w, int h, int n) { 
    ImprovedOM *feat = new ImprovedOM(n * n);
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
    //resize_mat_by_width(img_rgb, img_rgb, 160);

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

//    show_mat(saliency_map);
//    show_mat(cv::Mat(img, rect));

    // 4= extract om feature on yuv data
    //cv::Rect rect = margin;
    ImprovedOM *feat = new ImprovedOM(n * n);
    get_real_feature(data, w, h, rect, n, feat->_arr_color, feat->_arr_entropy);
    //feat->Print();


    // 5= get the index of feature
//    ExtractIndex(_arr_color, &idx_a);
//    ExtractIndex(_arr_entropy, &idx_b);

    return feat;
}

ImprovedOM* ImprovedOM::ReadFromFile(FILE *pf) {
    // un-beautiful codes
    uint64 key;
    int n;
    int ret = fread(&key, sizeof(uint64), 1, pf);
    if (ret != 1) {
        return NULL;
    }
    ret = fread(&n, sizeof(int), 1, pf);
    if (ret != 1) {
        return NULL;
    }

    ImprovedOM *feat = new ImprovedOM(n);
    feat->SetID(key);
    fread(feat->_arr_color, sizeof(uint8), n, pf);
    fread(feat->_arr_entropy, sizeof(uint8), n, pf);

    feat->ExtractBinaryIndex(feat->_arr_color, &feat->_binary_idx_color, n);
    feat->ExtractBinaryIndex(feat->_arr_entropy, &feat->_binary_idx_ent, n);

    //printf("?? %llu %llu\n", feat->_binary_idx_color, feat->_binary_idx_ent);
//    printf("??");
//    print_bits(feat->_binary_idx_color);

    return feat;
}

OM* ReadFeatureFromFile(FILE *pf, int type) {
    if (type & FEAT_OM) {
        return SimplyOM::ReadFromFile(pf);
    } else if ((type & FEAT_IMP_OM) || (type & FEAT_SA_OM)) {
        return ImprovedOM::ReadFromFile(pf);
    }

    return NULL;
}
} // namespace vcd
