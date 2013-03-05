#include "om.h"
#include "base/logging.h"
#include <stdio.h>
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cv.h>

//#define N 100 
//#define N2 8 
#define subN 0


int N = 25;
//int N2 = 8;

// the threshold of the entropy of the valid image
const float ENTROPY_THRESHOLD = 3.5;

struct position{
    int idx;
    float val;
};

// function of compare the position struct
int cmp(const void *pa, const void *pb) {
    position *posA = (position *)pa;
    position *posB = (position *)pb;

    return posA->val > posB->val;
}

// convert the position struct to the feature
bool pos2result(const position *pos, uint8 *result, int len) {
    for (int i = 0; i < len; ++i) {
        result[pos[i].idx] = static_cast<uint8>(i);
    }
    return true;
}

void get_rect_avg(const uint8 *data, int w, int h,
        int x, int y, int rw, int rh, float *avg) {
    int sum = 0;
    for (int i = y; i < y + rh; i++) {
        for (int j = x; j < x + rw; j++) {
    //        printf("%s %d, %d %d %d %d\n", __FILE__, __LINE__, w, y, i, j);
            assert(i < h && j < w);
            sum += data[i * w + j];
        }
    }

    *avg = (float) sum / (rw * rh);
}

void get_rect_entropy(const uint8 *data, int w, int h,
        int x, int y, int rw, int rh, float *entropy) {
    const int nbox = 64; 
    int num[nbox];
    for (int i = 0; i < nbox; i++) {
        num[i] = 0;
    }

    for (int i = y; i < y + rh; i++) {
        for (int j = x; j < x + rw; j++) {
            assert(i < h && j < w);
            int val = data[i * w + j];
            num[val * nbox / 256]++;
        }
    }

    int total = rw * rh;
    float val = 0.0; 
    for (int i = 0; i < nbox; i++) {
        float per = num[i] / (float)total;
        val += -1 * per * log(per + 0.000001);
    }

    *entropy = val;
}

const double k_ent_thres = 2.0;
const int k_n_border_split = 5;

// 
// in order to judge a border whether is a margin, need to calc 
// the entropy of the border. but the logo also appear in the 
// margin. if calc the whole entropy, the logo will make the 
// entropy large and get the wrong decision.
// so divide the border into n parts. and cacl entropy of each one.
// because the logo only appear in one or two(logo also is small).
// if entropy of k(k > n/2) parts is ok, can get this border is a margin. 
//
// input:
//      @row    must be 1 * n matrix
// ouput:
//      bool    true if in margin
//
bool check_border(const cv::Mat &row) {
    if (row.rows != 1) {
        VLOG(0, "check border error. must be 1*n");
        return false;
    }

    int each_size = row.cols / k_n_border_split;
    int ok = 0;
    float dev[k_n_border_split];
    for (int i = 0; i < k_n_border_split; ++i) {
        cv::Mat roi(row, cv::Rect(each_size * i, 0, each_size, 1));
        cv::Scalar std_dev, m; 
        cv::meanStdDev(roi, m, std_dev);
        if (std_dev.val[0] < k_ent_thres) {
            ok++;
        }
        dev[i] = std_dev.val[0];
    }
    //VLOG(1, "%d %f %f %f %f %f\n", ok, dev[0], dev[1], dev[2], dev[3], dev[4]);
    if (ok > k_n_border_split / 2) {
        return true;
    } else {
        return false;
    }
}

void remove_margin(const cv::Mat &data, cv::Rect *res) {
    int top, bottom, left, right;
    int i;
    
    // 1= find the top
    int top_upper = data.rows / 4;
    for (i = 0; i < top_upper; ++i) {
        cv::Mat row = data.row(i);
        if (check_border(row) == false) {
            break;
        }
    }
    top = i;

    // 2= find the bottom
    int bottom_upper = data.rows * 3 / 4;
    for (i = data.rows - 1; i > bottom_upper; --i) {
        cv::Mat row = data.row(i);
        if (check_border(row) == false) {
            break;
        }
    }
    bottom = i;

    // 3= find the left
    int left_upper = data.cols / 4;
    for (i = 0; i < left_upper; ++i) {
        cv::Mat col = data.col(i);
        cv::Mat row = col.t();
        if (check_border(row) == false) {
            break;
        }
    }
    left = i;

    // 3= find the right
    int right_upper = data.cols * 3 / 4;
    for (i = data.cols - 1; i > right_upper; --i) {
        cv::Mat col = data.col(i);
        cv::Mat row = col.t();
        if (check_border(row) == false) {
            break;
        }
    }
    right = i;

    VLOG(1, "%d %d %d %d", left, top, right, bottom);

    res->x = left;
    res->y = top;
    res->width = right - left + 1;
    res->height = bottom - top + 1;
}

#define STEP_SIZE 2
#define CHECK_POINT 50 
#define TH_LUMA 30
#define VARIA_VAL 500
#define BIN_THRESH 0.9
#define CBIN_THRESH 0.62

//#define _P(x) (x)
#define _P(x) 

inline int check_edge2(const int *sample, const int len) {
    const int nBin = 128;
    const int nClrBin = 64;
    int bin[nBin];
    int clrBin[nClrBin];
    for (int i = 0; i < nBin; i++) {
        bin[i] = 0;
    }
    for (int i = 0; i < nClrBin; i++) {
        clrBin[i] = 0;
    }

    for (int i = 0; i < len - 1; i++) {
        assert(sample[i] >= 0);
        clrBin[sample[i] * nClrBin / 255]++;
        bin[(sample[i + 1] - sample[i] + 255) * nBin / 510]++;
    }
    _P(printf("\n"));
    int count = 0;
    for (int i = 0; i < nBin; i++) {
        _P(printf("%.4g ", (float)bin[i] / len));
        if (bin[i] > 0) count++;
    }
    _P(printf("\n"));
    int count2 = 0;
    for (int i = 0; i < nClrBin; i++) {
        _P(printf("%d ", clrBin[i]));
        if (clrBin[i] > 5) count2++;
    }
    _P(printf("\n"));
    if (count2 <= 7 && count <= 2) return 0;
    if (count2 > 7) return 1;

    std::sort(bin, bin + nBin);
    std::sort(clrBin, clrBin + nClrBin);
    int sum = 0;
    int clrSum = 0;
    int thresh = len * BIN_THRESH;
    int thresh2 = len * CBIN_THRESH;
    int flag = 0;
    for (int i = nBin - 1, k = 2; k > 0; i--, k--) {
        sum += bin[i];     
        _P(printf("%.4g\n", (float)sum / len));
        if (sum > thresh) {
            flag = 1;
            break;
        }
    }
    _P(printf("color: %d\n", len));
    for (int i = nClrBin - 1, k = 3; flag && k > 0; i--, k--) {
        clrSum += clrBin[i];
        _P(printf("%d %.4g\n", clrSum, (float)clrSum / len));
        if (clrSum > thresh2) {
            return 0;
        }
    }

    return 1;
}

void get_edge2(const uint8 *data, int w, int h, int &sw, int &sh, int &pw, int &ph) { 
    int top = 0, bottom = h - 1;
    int left = 0, right = w - 1;

    int sampleLen = w > h ? w : h;
    int *sample = new int[sampleLen];
    // left
    _P(printf("---->left:\n"));
    for (int line = 0; line < w / 4; line += 2) {
        int k;
        for (k = 0; k < h; k++) {
            const uint8 *point = data + w * k + line;
            sample[k] = point[0];
        } 
        left = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //right
    _P(printf("---->right:\n"));
    for (int line = w - 1; line > 3 * w / 4; line -= 2) {
        int k;
        for (k = 0; k < h; k++) {
            const uint8 *point = data + w * k + line;
            sample[k] = point[0];
        }
        right = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //top
    _P(printf("---->top:\n"));
    for (int line = 0; line < h / 4; line += 2) {
        int k;
        const uint8 *point = data + line * w;
        for (k = 0; k < w; k++) {
            sample[k] = point[k];
        }
        top = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    //bottom
    _P(printf("---->bottom:\n"));
    for (int line = h - 1; line > 3 * h / 4; line -= 2) {
        int k;
        const uint8 *point = data + line * w;
        for (k = 0; k < w; k++) {
            sample[k] = point[k];
        }
        bottom = line;
        if (check_edge2(sample, k)) {
            break;
        }
    }

    sw = left;
    sh = top;
    pw = right - left + 1;
    ph = bottom - top + 1;
    //show_edge_image(data, w, h, left, right, top, bottom);

    delete [] sample;
}

inline int check_edge(int value) {
    int avg = value / (3 * (CHECK_POINT + 1));
    if (avg > TH_LUMA) {
        return 1;
    }
    return 0;
}

inline int check_edge(const int *sample, const int len) {
    const int binSize = 64;
    int bin[binSize];

    for (int i = 0; i < binSize; i++) 
        bin[i] = 0;

    for (int i = 0; i < len; i++) {
        bin[sample[i] * binSize / 255]++;
    }

    std::sort(bin, bin + binSize);
    int sum = 0;
    int thresh = len * BIN_THRESH;
    for (int i = binSize - 1, k = 3; k > 0; i--, k--) {
        sum += bin[i];
        printf("%.4g\n", (float)sum / len);
        if (sum > thresh) {
            printf("not edge\n");
            return 0;
        }
    }

    return 1;
}

//
// check the entropy of the center of the image
// @data    the input image
// @w       the width
// @h       the height
// @sw,@wh  the top and left of the center of the image
//
int check_image_entropy(const uint8 *data, int w, int h,
                        int sw, int sh, int pw, int ph) {
    const int box = 256;
    int num[box];

    for (int i = 0; i < box; i++)
        num[i] = 0;

    for (int i = sh; i < sh + ph; i++) {
        for (int j = sw; j < sw + pw; j++)
            num[data[i * w + j] * box / 256]++;
    }

    float sum = 0;
    int total = pw * ph;

    for (int i = 0; i < box; i++) {
        //printf(" %d", num[i]);
        float per = (float)num[i] / (float)total;    
        sum += -1.0 * per * log(per + 0.000001);
    }

    printf("%.3f\n", sum);

    if (sum < ENTROPY_THRESHOLD) {
        return 0;
    } else {
        return 1;
    }
}

// check the entropy of the full image
int check_image_entropy(const uint8 *data, int w, int h) {
    return check_image_entropy(data, w, h, 0, 0, w, h);
}

void get_real_feature(const uint8 *data, int w, int h, int N2,
        uint8 *orderAvg, uint8 *orderEnt) {
    int sw = 0, sh = 0, pw = w, ph = h;
    get_edge2(data, w, h, sw, sh, pw, ph);
    cv::Rect rect(sw, sh, pw, ph);
    get_real_feature(data, w, h, cv::Rect(sw, sh, pw, ph), N2,
                     orderAvg, orderEnt);
}

void get_real_feature(const uint8 *data, int w, int h,
                      cv::Rect view, int N2,
                      uint8 *orderAvg, uint8 *orderEnt) {
    /*
     * we only use the middle center area of the image
     *  ____________
     *  | _______  |
     *  | |      | |
     *  | |center| |
     *  | |______| |
     *  |__________|
     */

    int sw = 0, sh = 0, pw = w, ph = h;
    sw = view.x;
    sh = view.y;
    pw = view.width;
    ph = view.height;
    //printf("check center: %d %d %d %d %d %d\n", w, h, sw, sh, pw, ph);
    assert(sw >= 0 && sw + pw <= w);
    assert(sh >= 0 && sh + ph <= h);

    int subw = pw / N2;
    int subh = ph / N2;
    int len = N2 * N2;
    position *posAvg = NULL, *posEnt = NULL;
    if (orderAvg != NULL)
        posAvg = new position[len];
    if (orderEnt != NULL)
        posEnt = new position[len];

    int idx = 0;
    for (int i = 0; i < N2; i++) {
        for (int j = 0; j < N2; j++) {
            // divide the image and calc feature of 
            // each block
            
            int leftX = j * subw + sw;
            int leftY = i * subh + sh;

            if (orderAvg != NULL) {
                get_rect_avg(data, w, h, leftX, leftY, subw, subh,
                    &posAvg[idx].val);
                posAvg[idx].idx = i * N2 + j;
            }
            if (orderEnt != NULL) {
                get_rect_entropy(data, w, h, leftX, leftY, subw, subh,
                    &posEnt[idx].val);
                posEnt[idx].idx = i * N2 + j;
            }
            idx++;
        }
    }

    if (orderAvg != NULL) {
        qsort(posAvg, len, sizeof(position), cmp);
        pos2result(posAvg, orderAvg, len);
    }
    if (orderEnt != NULL) {
        qsort(posEnt, len, sizeof(position), cmp);
        pos2result(posEnt, orderEnt, len);
    }

    if (orderAvg != NULL) {
        delete [] posAvg;
    }
    if (orderEnt != NULL)
        delete [] posEnt;
}
