#include "saliency.h"
#include <stdio.h>
#include <vector>
#include <math.h>

namespace vcd {

const uint32 HIST_NUM = 256;
const uint32 THRESHOLD = 127 * 127;
const uint32 kmaxcolorval = 256;

#define SQUERE(x) ((x) * (x))
#define GAUSSIAN_DIS(x, y, z) (sqrt(SQUERE(x) + SQUERE(y) + SQUERE(z)))

template<typename T>
static inline void copy_if_greater(T &old, T &fresh) {
    if (fresh > old) {
        old = fresh;
    }
}

Saliency::Saliency() {}

Saliency::~Saliency() {}

// here we assume the data only gray color
bool Saliency::ExtractSaliency(const uint8 *data, const int w,
                               const int h, uint8 *result) {
    if (data == NULL) {
        return false;
    }

    uint8 *quantize_map = new uint8[w * h];
    if (QuantizedBlockedImage(data, w, h, quantize_map) == false) {
        return false;
    }

    int32 hist[HIST_NUM] = {0};
    uint8 color_dis[kmaxcolorval][kmaxcolorval];
    uint32 score[HIST_NUM];
    uint32 N = w * h;
    const uint8 *ptr = quantize_map;

    // pre calc the distance of diff color
    for (uint32 i = 0; i < kmaxcolorval; ++i) {
        for (uint32 j = i; j < kmaxcolorval; ++j) {
            color_dis[i][j] = color_dis[j][i] = GetDistance(i, j);
        }
    }
    
    // count num of each color
    for (uint32 i = 0; i < static_cast<uint32>(w * h); ++i) {
        hist[ptr[i]]++;
    }

    // calc score
    uint32 max_score = 0;
    for (uint32 i = 0; i < HIST_NUM; ++i) {
        score[i] = 0;
        for (uint32 j = 0; j < HIST_NUM; ++j) {
            if (i == j) continue;
            score[i] += hist[j] * color_dis[i][j];
        }
        score[i] /= N;
        copy_if_greater(max_score, score[i]);
    }

    for (uint32 i = 0; i < N; ++i) {
        result[i] = score[ptr[i]] * 255 / max_score;
        //printf("??%d %d\n", score[ptr[i]], result[i]);
    }

    //ExtractSaliency3(quantize_map, w, h, result);

    delete [] quantize_map;

    return true;
}

bool Saliency::ExtractSaliencyWithColor(const uint8 *data, const int w,
                                        const int h, uint8 *result) {
    if (data == NULL) {
        fprintf(stderr, "NULL Pointer! %s %d\n", __FILE__, __LINE__);
        return false;
    }

    int32 *score = new int32[w * h];
    int32 score_max = 0;
    const int32 kblocksize = 15;
    for (int32 i = 0; i < h; ++i) {
        for (int32 j = 0; j < w; ++j) {
            score[i * w + j] = 0;
            // [a] because the color store in data is
            // b1g1r1b2g2r2...bngnrn
            // so the idx of (i,j) point in data
            // is [i * 3w + 3j]
            int32 my = i * 3 * w + j * 3;
            printf("??%d ", i * w + j);
            
            for (int32 nx = i - kblocksize; nx < i + kblocksize; ++nx) {
                if (nx < 0 || nx >= h) {
                    continue;
                }
                for (int32 ny = j - kblocksize; ny < i + kblocksize; ++ny) {
                    if (ny < 0 || ny >= w) {
                        continue;
                    }
                    if (i == nx && j == ny) {
                        continue;
                    }

                    // same reason as [a]
                    int32 np = nx * 3 * w + ny * 3;
                    score[i * w + j] += GAUSSIAN_DIS(data[my] - data[np],
                                                     data[my + 1] - data[np + 1],
                                                     data[my + 2] - data[np + 2]);
                }
            }
            // record the max score in order to normalize
            copy_if_greater(score_max, score[i * w + j]);
            printf("\t\t%d %d\n", score_max, score[i * w + j]);
        }
    }

    // normalize
    const int32 kmaxval = 255;
    for (int32 i = 0; i < h * w; ++i) {
        result[i] = score[i] * kmaxval / score_max;
    }

    delete [] score;

    return true;
}

bool Saliency::ExtractSaliency2(const uint8 *data, const int w,
                                const int h, uint8 *result) {
    if (data == NULL) {
        fprintf(stderr, "NULL Pointer! %s %d\n", __FILE__, __LINE__);
        return false;
    }

    const uint8 n = 8;
    uint32 block[n][n];

    int sub_w = w / n;
    int sub_h = h / n;

    for (int x = 0; x < n; ++x) {
        for (int y = 0; y < n; ++y) {
            block[x][y] = 0;
            for (int q = x * sub_h; q < (x + 1) * sub_h; ++q) {
                for (int p = y * sub_w; p < (y + 1) * sub_w; ++p) {
                    //printf("??%d\n", data[q * w + p]);
                    block[x][y] += data[q * w + p];
                }
            }
            //printf("%d\n", block[x][y]);
            block[x][y] /= (sub_w * sub_h);
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", block[i][j]);
        }
        printf("\n");
    }

    int32 simple[n][n];
    int max_sim_val = 0;
    for (int i = 0; i < n * n; ++i) {
        int x = i / n;
        int y = i % n;

        simple[x][y] = 0;
        for (int j = 0; j < n * n; ++j) {
            if (i == j) continue;
            int t_x = j / n;
            int t_y = j % n;
            simple[x][y] += GetDistance(block[x][y], block[t_x][t_y]);
        }
        simple[x][y] /= (n * n);
//        if (simple[x][y] > max_sim_val) {
//            max_sim_val = simple[x][y];
//        }
        copy_if_greater(max_sim_val, simple[x][y]);
    }

    printf("<<<<<<<<<<<<<\n");
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            printf("%d ", simple[i][j] * 255 / max_sim_val);
        }
        printf("\n");
    }

    return true;
}

bool Saliency::ExtractSaliency3(const uint8 *data, const int w,
                                const int h, uint8 *result) {
    if (data == NULL) {
        fprintf(stderr, "NULL Pointer! %s %d\n", __FILE__, __LINE__);
        return false;
    }

    int32 n = w * h;
    int32 *score = new int32[n];
    int32 kblocksize = 9;
    int32 max_score = 0;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            int32 r = 0;
            for (int x = i - kblocksize; x < i + kblocksize; ++x) {
                if (x < 0 || x >= h) continue;
                for (int y = j - kblocksize; y < j + kblocksize; ++y) {
                    if (y < 0 || y >= w) continue;
                    r += GetDistance(data[i * w + j], data[x * w + y]);
                }
            }
            score[i * w + j] = r;
            copy_if_greater(max_score, r);
        }
    }

    // normalized
    for (int i = 0; i < w * h; ++i) {
        result[i] = score[i] * 255 / max_score;
    }

    delete [] score;

    return true;
}

bool Saliency::QuantizedBlockedImage(const uint8 *data, const int w,
                                     const int h, uint8 *result) {
    if (data == NULL) {
        fprintf(stderr, "NULL Pointer! %s %d\n", __FILE__, __LINE__);
        return false;
    }

    const int kblocksize = 5;
    for (int i = 0; i < h;) {
        for (int j = 0; j < w;) {
            int total = 0;
            for (int x = i; x < i + kblocksize && x < h; ++x) {
                for (int y = j; y < j + kblocksize && y < w; ++y) {
                    total += data[x * w + y];
                }
            }
            total /= (kblocksize * kblocksize);
            for (int x = i; x < i + kblocksize && x < h; ++x) {
                for (int y = j; y < j + kblocksize && y < w; ++y) {
                    result[x * w + y] = total;
                }
            }
            j += kblocksize;
        }
        i += kblocksize;
    }

    return true;
}

// handle the pic, and return the 
// the main area of objects in the picture.
bool Saliency::ExtractArea(const uint8 *data, const int w, const int h,
                           uint8 *result) {
    if (data == NULL) {
        return false;
    }

    const uint32 N = w * h;
    uint8 *sa_map = new uint8[w * h]; 

    ExtractSaliency(data, w, h, sa_map);

    // copy the result to the input memory;
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
    uint8 abs_val = a > b ? a - b : b - a;
    return abs_val;
    //return SQUERE(abs_val);
}

bool Saliency::IsMaxVal(const uint8 *data, const int w,
                        const int cw, const int ch, const int size) {
    const uint8 c_val = data[ch * w + cw];
    for (int32 j = ch - size; j <= ch + size; ++j) {
        for (int32 i = cw - size; i <= cw + size; ++i) {
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
