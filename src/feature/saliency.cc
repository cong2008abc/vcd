#include "saliency.h"
#include <stdio.h>
#include <vector>

namespace vcd {

const uint32 HIST_NUM = 256;
const uint32 THRESHOLD = 127 * 127;

#define SQUERE(x) ((x) * (x))

Saliency::Saliency() {}

Saliency::~Saliency() {}

// here we assume the data only gray color
bool Saliency::ExtractSaliency(const uint8 *data, const int w, const int h, uint8 *result) {
    if (data == NULL) {
        return false;
    }

    int32 hist[HIST_NUM] = {0};
    for (uint32 i = 0; i < w * h; ++i) {
        hist[data[i]]++;
    }

    uint32 MaxColorVal = 256;
    uint8 dis[HIST_NUM][HIST_NUM];
    for (uint32 i = 0; i < MaxColorVal; ++i) {
        for (uint32 j = i; j < MaxColorVal; ++j) {
            dis[i][j] = dis[j][i] = GetDistance(i, j);
        }
    }

    uint32 score[HIST_NUM];
    for (uint32 i = 0; i < HIST_NUM; ++i) {
        score[i] = 0;
        for (uint32 j = 0; j < HIST_NUM; ++j) {
            if (i == j) continue;
            score[i] += hist[j] * dis[i][j];
        }
    }

    uint32 N = w * h;
    for (uint32 i = 0; i < N; ++i) {
        result[i] = score[data[i]] / N;
    }

    return true;
}

bool Saliency::ExtractArea(const uint8 *data, const int w, const int h,
                           uint8 *result) {
    if (data == NULL) {
        return false;
    }

    const uint32 N = w * h;
    uint8 *sa_map = new uint8[w * h]; 

    ExtractSaliency(data, w, h, sa_map);

    for (uint32 i = 0; i < N; ++i) {
        result[i] = 0;
    }

    for (uint32 i = 0; i < N; ++i) {
        if (data[i] > 200) {
            result[i] = 255;
        }
    }

    /*
    std::vector<Point> max_pixel; 
    uint32 local_size = 10;
    for (uint32 i = local_size; i < h - local_size; ++i) {
        for (uint32 j = local_size; j < w - local_size; ++j) {
            if (IsMaxVal(data, w, j, i, local_size)) {
            //    printf("%d\n", data[i * w + j]);
                max_pixel.push_back(Point(j, i));
            }
        }
    }

    for (size_t t = 0; t < max_pixel.size(); ++t) {
        int32 top = 0, left = 0;
        int32 area_w = 0, area_h = 0;

        ExtendArea(sa_map, w, h, max_pixel[t], &left, &top, &area_w, &area_h); 
        printf("extend ret: %d %d %d %d\n", top, left, area_w, area_h);

        // draw the area into the reture result
        for (int32 i = 0; i < area_h; ++i) {
            for (int32 j = 0; j < area_w; ++j) {
                int32 x = left + j;
                int32 y = top + i;

                result[y * w + x] = 255;
            }
        }
    }
    */

    delete [] sa_map;
    return true;
}

uint8 Saliency::GetDistance(uint8 a, uint8 b) {
    return a > b ? a - b : b - a;
}

bool Saliency::IsMaxVal(const uint8 *data, const int w, const int cw, const int ch, const int size) {
    const uint8 c_val = data[ch * w + cw];
    for (uint32 j = ch - size; j <= ch + size; ++j) {
        for (uint32 i = cw - size; i <= cw + size; ++i) {
            if (data[w * j + i] >= c_val &&
                (i != cw || j != ch)) 
                return false;
        }
    }
    return true;
}

/*
 * the start area is a 3*3 square. and grow up by 1 pixel for each side
 */
bool Saliency::ExtendArea(const uint8 *data, const int w, const int h,
                          const Point &center, 
                          int *left_, int *top_, int *area_w_, int *area_h_) {
    if (left_ == NULL || top_ == NULL || 
        area_w_ == NULL || area_h_ == NULL) {
        return false;
    }

    int32 start_area_size = 1;    
    int32 top[2], bottom[2], left[2], right[2];
    top[0] = center.y - start_area_size;
    bottom[0] = center.y + start_area_size;
    left[0] = center.x - start_area_size;
    right[0] = center.x + start_area_size;
    int32 flag = 0;
    bool update = false;

    // extend loop
    while (1) {
        // 1= extend top side     
        if (top[flag] != 0 &&
            IfExtendRow(data, w, h, left[flag], right[flag],
                        top[flag] - 1, top[flag])) {
            top[1 - flag] = top[flag] - 1;
            update = true;
        } else {
            top[1 - flag] = top[flag];
        }

        // 2= extend bottom side
        if (bottom[flag] != h - 1 && 
            IfExtendRow(data, w, h, left[flag], right[flag],
                        bottom[flag] + 1, bottom[flag])) {
            bottom[1 - flag] = bottom[flag] + 1;
            update = true;
        } else {
            bottom[1 - flag] = bottom[flag];
        }

        // 3= extend left side
        if (left[flag] != 0 &&
            IfExtendCol(data, w, h, top[flag], bottom[flag],
                        left[flag] - 1, left[flag])) {
            left[1 - flag] = left[flag] - 1;
            update = true;
        } else {
            left[1 - flag] = left[flag];
        }
        
        // 4= extend right side
        if (right[flag] != w - 1 &&
            IfExtendCol(data, w, h, top[flag], bottom[flag],
                        right[flag] + 1, right[flag])) {
            right[1 - flag] = right[flag] + 1;
            update = true;
        } else {
            right[1 - flag] = right[flag];
        }

        if (update == false) break;
        
        // update the status
        flag = 1 - flag;
        update = false;
    }

    // return the result
    *left_ = left[0];
    *top_ = top[0];
    *area_w_ = right[0] - left[0] + 1;
    *area_h_ = bottom[0] - top[0] + 1;

    return true;
}

bool Saliency::IfExtendRow(const uint8 *data, const int w, const int h,
                           const int left, const int right,
                           const int row_out, const int row_in) {
    if (row_in < 0 || row_in >= h) {
        return false;
    }

    uint32 sum_out = 0;
    uint32 sum_in = 0;

    const uint8 *out_ptr = data + row_out * w + left;
    const uint8 *in_ptr = data + row_in * w + left;
    
    int32 x = left;
    while (x++ <= right) {
        sum_in += *in_ptr++;
        sum_out += *out_ptr++;
    }

    uint32 border_len = right - left + 1;
    uint32 eq_left = THRESHOLD * SQUERE(border_len);
    //printf("?? %d %d\n", eq_left, sum_out * sum_in);

    if (eq_left < sum_in * sum_out) {
        return true;
    } else {
        return false;
    }
}

bool Saliency::IfExtendCol(const uint8 *data, const int w, const int h,
                           const int top, const int bottom,
                           const int col_out, const int col_in) {
    if (col_in < 0 || col_in >= w) {
        return false;
    }

    uint32 sum_out = 0;
    uint32 sum_in = 0;

    const uint8 *out_ptr = data + top * w + col_out;
    const uint8 *in_ptr = data + top * w + col_in;
    
    int32 x = top;
    while (x++ <= bottom) {
        sum_in += *in_ptr;
        sum_out += *out_ptr;

        in_ptr += w;
        out_ptr += w;
    }

    uint32 border_len = bottom - top + 1;
    uint32 eq_left = THRESHOLD * SQUERE(border_len);
    uint32 eq_right = sum_out * sum_in;
    printf("??? %d %d\n", eq_left, eq_right);

    if (eq_left < sum_in * sum_out) {
        return true;
    } else {
        return false;
    }
}

} // namespace vcd
