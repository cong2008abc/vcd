#include "feature.h"
#include "om.h"
#include "img_saliency.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>
#include <cv.h>

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

bool Feature::CvtYUV2BGR(const uint8 *data_, int w, int h,
                         IplImage *rgb) {
    IplImage *y = cvCreateImageHeader(cvSize(w, h), IPL_DEPTH_8U, 1); 
    IplImage *u = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1); 
    IplImage *v = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1); 

    IplImage *hu = cvCreateImageHeader(cvSize(w/2, h/2), IPL_DEPTH_8U, 1); 
    IplImage *hv = cvCreateImageHeader(cvSize(w/2, h/2), IPL_DEPTH_8U, 1); 

    uint8 *data = const_cast<uint8*>(data_);
    cvSetData(y, data, w);
    cvSetData(hu, data + w * h, w / 2);
    cvSetData(hv, data + static_cast<int>(w * h * 1.25), w / 2);

    cvResize(hu, u, CV_INTER_LINEAR);
    cvResize(hv, v, CV_INTER_LINEAR);

    cvMerge(y, u, v, NULL, rgb);
    cvCvtColor(rgb, rgb, CV_YUV2BGR);

    cvReleaseImage(&u);
    cvReleaseImage(&v);
    cvReleaseImageHeader(&y);
    cvReleaseImageHeader(&hu);
    cvReleaseImageHeader(&hv);

    return true;
}


/*
 * implement ImproveOMFeature class
 */
int ImpOMFeature::FEATURE_LEN = 16;

ImpOMFeature::ImpOMFeature() {
    _arr_color = NULL;
    _arr_entropy = NULL;
}

ImpOMFeature::~ImpOMFeature() {
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h) {
    return ExtractFrame(data, w, h, 4);
}

bool ImpOMFeature::ExtractFrame(const uint8 *data, int w, int h, int n) {
    _arr_color = new uint8[n * n];
    //_arr_entropy = NULL;
    _arr_entropy = new uint8[n * n];
    
    // 1= cvt the yuv data to rgb
    IplImage *img_rgb = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
    if (CvtYUV2BGR(data, w, h, img_rgb) == false) {
        return false;
    }

    cv::Mat img = img_rgb;
    cv::Rect margin;
    remove_margin(img, &margin);

    // 2= extract the main rectangle on rgb image
    cv::Mat roi(img, margin);
    cv::Rect rect;
    cv::Mat saliency_map;
    Saliency::Get(roi, saliency_map);    
    Saliency::ExtractView(saliency_map, rect);
    
//    show_mat(roi);
//    cv::rectangle(saliency_map, rect, cv::Scalar(255));
//    show_mat(saliency_map);
    //Saliency::Evaluate(saliency_map, saliency_map);
//    show_mat(saliency_map);
//    show_yuv(data, w, h);

    // 3= extract om feature on yuv data
    get_real_feature(data, w, h, rect, n, _arr_color, _arr_entropy);

    cvReleaseImage(&img_rgb);
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

float ImpOMFeature::Compare(const Feature *rf) {
    //TODO(zhouchao) compare two feature distance
    //...

    const ImpOMFeature *ptr_r = dynamic_cast<const ImpOMFeature*>(rf);
    float ret_color = InterCompare(_arr_color, ptr_r->_arr_color);
    float ret_entropy = InterCompare(_arr_entropy, ptr_r->_arr_entropy);

    return ret_color > ret_entropy ? ret_entropy : ret_color;
}

/*
 * this is compare method may be wrong!
 */
float ImpOMFeature::InterCompare(const uint8 *arr_a, const uint8 *arr_b) {
    uint8 s[ImpOMFeature::FEATURE_LEN], d[ImpOMFeature::FEATURE_LEN];
    int len = ImpOMFeature::FEATURE_LEN;
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
