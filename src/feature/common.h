#ifndef _COMMON_H_
#define _COMMON_H_

#include <vector>
#include <cv.h>

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
inline T pntSqrDist(const cv::Point_<T> &p1, const cv::Point_<T> &p2) {
    return sqr(p1.x - p2.x) + sqr(p1.y - p2.y);
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

#endif
