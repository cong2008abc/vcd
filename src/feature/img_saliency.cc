#include "img_saliency.h"
#include "define.h"
#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <algorithm>
#include <map>
#include <assert.h>

namespace vcd {

typedef std::vector<double> vecD;
typedef std::vector<float> vecF;

template<class T>
inline T sqr(T x) {
    return x * x;
}

template<class T>
inline T vecDist3(const cv::Vec<T, 3> &v1, const cv::Vec<T, 3> &v2) {
    return sqrt(sqr(v1[0] - v2[0]) + sqr(v1[1] - v2[1]) +
                sqr(v1[2] - v2[2]));
}

template<class T>
inline T vecSqrDist3(const cv::Vec<T, 3> &v1, const cv::Vec<T, 3> &v2) {
    return sqr(v1[0] - v2[0]) + sqr(v1[1] - v2[1]) + sqr(v1[2] - v2[2]);
}

template<class T1, class T2>
inline void operator /= (cv::Vec<T1, 3> &v1, T2 v2) {
    v1[0] /= v2;
    v1[1] /= v2;
    v1[2] /= v2;
}

template<class T>
inline double get_factor_a(T x, T a, T u) {
    if (x <= u) {
        return 0.0f;
    } else if (x >= a) {
        return 1.0f;
    } else {
        return (x - u) / (double)(a - u);
    }
}

template<class T>
inline double get_factor_u(T x, T a, T u) {
    if (x <= u) {
        return 1.0f;
    } else if (x >= a) {
        return 0.0f;
    } else {
        return (x - a) / (double)(u - a);
    }
}

template<class T>
inline T _abs(T x) {
    return x > 0 ? x : -x;
}

bool Saliency::Get(const cv::Mat &src, cv::Mat &result) {
    cv::Mat sal, img3f;
    src.convertTo(img3f, CV_32FC3, 1.0 / 255);
    sal = GetHC(img3f);

    sal.convertTo(result, CV_8U, 255);
//    sal.convertTo(sal, CV_8U);
//    unsigned char *ptr = (unsigned char*)(sal.data);
//    for (int i = 0; i < 10; ++i) {
//        printf("%d\n", ptr[i]);
//    }

    return true;
}

bool Saliency::Evaluate(const cv::Mat &src, cv::Mat &result) {
    return ExtractView(src, result);
    // here need val of src between [0, 255]
    cv::Mat img1d = src;   

    const int kmaxval = 255;
    int hist[kmaxval] = {0};
    int n = img1d.rows * img1d.cols;
    double proba[kmaxval] = {0.f};

    uint8 *ptr = (uint8*)(img1d.data);
    uint8 *end = ptr + n;
    while (ptr < end) {
        hist[*ptr++]++;
    }

    int target_u, target_a;
    double min_diff = 1000000.0f;
    for (int u = 0; u <= kmaxval; ++u) {
        for (int a = u + 1; a <= kmaxval; ++a) {     
            double ha = 0.0f, hu = 0.0f;
            double pb_a = GetProba(hist, kmaxval, a, u, get_factor_a) + 0.000001f;    
            double pb_u = GetProba(hist, kmaxval, a, u, get_factor_u) + 0.000001f;    
            for (int i = 0; i < kmaxval; ++i) {
                ha += (hist[i] / pb_a) * log(hist[i] / pb_a);
                hu += (hist[i] / pb_u) * log(hist[i] / pb_u);
            }

            //printf("--%d %d %f %f %f %f %f\n",u,a, pb_a, pb_u, ha, hu, _abs(ha - hu));

            if (_abs(ha - hu) < min_diff) {
                min_diff = _abs(ha - hu);
                target_a = a;
                target_u = u;
            }
        }
    }

    printf("?? result a:%d u:%d min:%f\n", target_a, target_u, min_diff);

    result = cv::Mat::zeros(src.size(), 8U);
    int *res_ptr = (int*)(result.data);

    // next steps
    // 1) find the start points[need a)local maximum, b)belong to attended areas]
    // 2) extend the area from the start points, follow blow:
    //      c[i,j] < c[start_point] && c[i,j] > (a + u)/2
    //
    // the question is how to decide the start points.
    // because it need to be the attended area.

    return true;
}

cv::Mat Saliency::GetHC(const cv::Mat &img3f) {
    CV_Assert(img3f.data != NULL);
    cv::Mat idx1i, bin_color3f, color_nums1i, weights1f, _color_sal;
    Quantize(img3f, idx1i, bin_color3f, color_nums1i);
    cv::cvtColor(bin_color3f, bin_color3f, CV_BGR2Lab);

    cv::normalize(color_nums1i, weights1f, 1, 0, cv::NORM_L1, CV_32F);
    GetHC(bin_color3f, weights1f, _color_sal);
    float *color_sal = (float*)(_color_sal.data);
    cv::Mat sal_HC1f(img3f.size(), CV_32F);
    for (int r = 0; r < img3f.rows; ++r) {
        float *sal_v = sal_HC1f.ptr<float>(r);
        int *_idx = idx1i.ptr<int>(r);
        for (int c = 0; c < img3f.cols; ++c) {
            sal_v[c] = color_sal[_idx[c]];
        }
    }
    cv::GaussianBlur(sal_HC1f, sal_HC1f, cv::Size(3, 3), 0);
    cv::normalize(sal_HC1f, sal_HC1f, 0, 1, cv::NORM_MINMAX);

    return sal_HC1f;
}

bool Saliency::GetHC(const cv::Mat &bin_color3f, const cv::Mat &weight1f,
                     cv::Mat &_color_sal) {
    int nbin = bin_color3f.cols;
    _color_sal = cv::Mat::zeros(1, nbin, CV_32F);
    float *color_sal = (float*)(_color_sal.data);
    std::vector<std::vector<CostfIdx> >similar(nbin);
    cv::Vec3f *color = (cv::Vec3f*)(bin_color3f.data);
    float *w = (float*)(weight1f.data);
    for (int i = 0; i < nbin; ++i) {
        std::vector<CostfIdx> &sim = similar[i];
        sim.push_back(std::make_pair(0.f, i));
        for (int j = 0; j < nbin; ++j) {
            if (i == j) continue;

            float dij = vecDist3<float>(color[i], color[j]);
            sim.push_back(std::make_pair(dij, j));
            color_sal[i] += w[j] * dij;
        }
        std::sort(sim.begin(), sim.end());
    }

    SmoothSaliency(bin_color3f, _color_sal, 4.0f, similar);

    return true;
}

int Saliency::Quantize(const cv::Mat &img3f, cv::Mat &idx1i, cv::Mat &_color3f,
                        cv::Mat &_color_num, double ratio) {
    static const int clrNums[3] = {12, 12, 12};
    static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
    static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

//    if (img3f.data == NULL) {
//        printf("Error!\n");
//        return 1;
//    }
    //cv::CV_Assert(img3f.data != NULL);
    idx1i = cv::Mat::zeros(img3f.size(), CV_32S);
    int rows = img3f.rows, cols = img3f.cols;
    if (img3f.isContinuous() && idx1i.isContinuous())
    {
        cols *= rows;
        rows = 1;
    }

    // Build color pallet
    std::map<int, int> pallet;
    for (int y = 0; y < rows; y++)
    {
        const float* imgData = img3f.ptr<float>(y);
        int* idx = idx1i.ptr<int>(y);
        for (int x = 0; x < cols; x++, imgData += 3)
        {
            idx[x] = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
            pallet[idx[x]] ++;
        }
    }

    // Fine significant colors
    int maxNum = 0;
    {
        int count = 0;
        std::vector<std::pair<int, int> > num; // (num, color) pairs in num
        num.reserve(pallet.size());
        for (std::map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
             num.push_back(std::pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
        std::sort(num.begin(), num.end(), std::greater<std::pair<int, int> >());

        maxNum = (int)num.size();
        int maxDropNum = cvRound(rows * cols * (1-ratio));
        for (int crnt = num[maxNum-1].first; crnt < maxDropNum && maxNum > 1; maxNum--)
        {
            crnt += num[maxNum - 2].first;
        }
        maxNum = std::min(maxNum, 256); // To avoid very rarely case
        if (maxNum < 10)
            maxNum = std::min((int)pallet.size(), 100);
        pallet.clear();
        for (int i = 0; i < maxNum; i++)
            pallet[num[i].second] = i; 

        std::vector<cv::Vec3i> color3i(num.size());
        for (unsigned int i = 0; i < num.size(); i++)
        {
            color3i[i][0] = num[i].second / w[0];
            color3i[i][1] = num[i].second % w[0] / w[1];
            color3i[i][2] = num[i].second % w[1];
        }

        for (unsigned int i = maxNum; i < num.size(); i++)
        {
            int simIdx = 0, simVal = INT_MAX;
            for (int j = 0; j < maxNum; j++)
            {
                int d_ij = vecSqrDist3(color3i[i], color3i[j]);
                if (d_ij < simVal)
                    simVal = d_ij, simIdx = j;
            }
            pallet[num[i].second] = pallet[num[simIdx].second];
        }
    }

    _color3f = cv::Mat::zeros(1, maxNum, CV_32FC3);
    _color_num = cv::Mat::zeros(_color3f.size(), CV_32S);

    cv::Vec3f* color = (cv::Vec3f*)(_color3f.data);
    int* colorNum = (int*)(_color_num.data);
    for (int y = 0; y < rows; y++) 
    {
        const cv::Vec3f* imgData = img3f.ptr<cv::Vec3f>(y);
        int* idx = idx1i.ptr<int>(y);
        for (int x = 0; x < cols; x++)
        {
            idx[x] = pallet[idx[x]];
            color[idx[x]] += imgData[x];
            colorNum[idx[x]] ++;
        }
    }
    for (int i = 0; i < _color3f.cols; i++)
        color[i] /= colorNum[i];

    return _color3f.cols;
}

bool Saliency::SmoothSaliency(const cv::Mat &bin_color3f, cv::Mat &sal1d,
                              float delta,
                              const std::vector<std::vector<CostfIdx> > &similar) {
    if (sal1d.cols < 2) {
        return false;
    }
    //cv::CV_Assert(bin_color3f.size() == sal1d.size() && sal1d.rows == 1);
    int nbin = bin_color3f.cols;
    cv::Vec3f *color = (cv::Vec3f*)(bin_color3f.data);
    cv::Mat tmp_sal;
    sal1d.copyTo(tmp_sal);
    float *sal = (float*)(tmp_sal.data);
    float *nSal = (float*)(sal1d.data);

    int n = std::max(cvRound(nbin / delta), 2);
    vecF dist(n, 0), val(n);
    for (int i = 0; i < nbin; ++i) {
        const std::vector<CostfIdx> &sim = similar[i];
        float totalDist = 0;

        val[0] = sal[i];
        for (int j = 1; j < n; ++j) {
            int ithIdx = sim[j].second;
            dist[j] = sim[j].first;
            val[j] = sal[ithIdx];
            totalDist += dist[j];
        }
        float valCrnt = 0;
        for (int j = 0; j < n; ++j) {
            valCrnt += val[j] * (totalDist - dist[j]);
        }

        nSal[i] = valCrnt / ((n - 1) * totalDist);
    }

    return true;
}

double Saliency::GetProba(const int *hist, const int kmaxval,
                          int a, int u,
                          double (*fun)(int, int, int)) {
    double sum = 0;
    for (int i = 0; i < kmaxval; ++i) {
        sum += ((*fun)(i, a, u)) * hist[i];
    }
    return sum;
}

bool Saliency::ExtractView(const cv::Mat &_src, cv::Mat &result) {
    const cv::Mat *src = &_src;
    const float delta = 1.9f;

    //int n = src->cols * src->rows;
    int n = src->cols * src->rows;
    uint8 *ptr_sal = (uint8*)(src->data);

    // calc the sum of all the saliency map
    uint8 *t = ptr_sal;
    int loop = n;
    uint64 CM = 0;
    while (loop--) {
        CM += *t++;
    //    printf("%d\n", CM);
    }

    printf("??ok, %lld\n", CM);

    // precalc the sum of the cols and rows,
    int *sum_cols = new int[src->cols];
    int *sum_rows = new int[src->rows];
    for (int i = 0; i < src->rows; ++i) {
        int tmp = 0;
        for (int j = 0; j < src->cols; ++j) {
            tmp += ptr_sal[i * src->cols + j];
        }
        sum_rows[i] = tmp;
    }
    for (int j = 0; j < src->cols; ++j) {
        int tmp = 0;
        for (int i = 0; i < src->rows; ++i) {
            tmp += ptr_sal[i * src->cols + j];
        }
        sum_cols[j] = tmp;
    }

    // calc x0 and y0
    uint64 _x0 = 0, _y0 = 0;
    int x0 = 1, y0 = 0;
    for (int i = 0; i < src->rows; ++i) {
        _y0 += sum_rows[i] * (i + 1);
        //printf("???%llx %d\n", _y0, sum_rows[i]);
    }
    for (int j = 0; j < src->cols; ++j) {
        _x0 += sum_cols[j] * (j + 1);
    }
    y0 = static_cast<int>(_y0 / CM - 1);
    x0 = static_cast<int>(_x0 / CM - 1);

    printf("%d %d %d %d\n", src->rows,src->cols, x0, y0);
    assert(x0 >= 0 && x0 < src->cols);
    assert(y0 >= 0 && y0 < src->rows);

    uint64 _w_sum = 0, _h_sum = 0;
    for (int i = 0; i < src->rows; ++i) {
        _h_sum += sum_rows[i] * _abs(i - y0);
    }
    for (int j = 0; j < src->cols; ++j) {
        _w_sum += sum_cols[j] * _abs(j - x0);
    }

    int w = 2 * delta * static_cast<int>(_w_sum / CM);
    int h = 2 * delta * static_cast<int>(_h_sum / CM);

    printf("--result x0:%d y0:%d w:%d h:%d\n", x0, y0, w, h);

    /*
     * do by opencv functions!
     */
//    cv::Mat _rows = cv::Mat::zeros(1, src->rows, CV_32F);
//    cv::Mat _cols = cv::Mat::zeros(1, src->cols, CV_32F);
//    for (int i = 0; i < src->cols; ++i) {
//        _rows = _rows + src->col(i) * (i + 1);
//    }
//    for (int i = 0; i < src->rows; ++i) {
//        _cols = _cols + src->row(i) * (i + 1);
//    }

    // draw the rectangle
    result = _src;
    int left = x0 - w / 2;
    int top = y0 - h / 2;
    int right = x0 + w / 2;
    int bottom = y0 + h / 2;
    printf(">>%d %d %d %d\n", left, top, right, bottom);

    left = left < 0 ? 0 : left;
    top = top < 0 ? 0 : top;
    right = right >= result.cols ? result.cols - 1 : right;
    bottom = bottom >= result.rows ? result.rows - 1 : bottom;

    uint8 *ptr_res = (uint8*)(result.data);
    for (int i = top; i < bottom; ++i) {
        ptr_res[i * result.cols + left] = 255; 
        ptr_res[i * result.cols + right] = 255;
    }
    for (int i = left; i < right; ++i) {
        ptr_res[top * result.cols + i] = 255;
        ptr_res[bottom * result.cols + i] = 255;
    }

    delete [] sum_cols;
    delete [] sum_rows;

    return true;
}

} // namespace vcd
