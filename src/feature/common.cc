#include "common.h"
#include "utils.h"
#include <cv.h>

namespace vcd {

int get_ipl_data(const IplImage *src, int nChannel, uint8 *data) {
    int w = src->width;
    int h = src->height;
    int n = src->nChannels;
    int ws = src->widthStep;

    int idx = 0;
    for (int i = 0; i < h; ++i) {
        uint8 *ptr = reinterpret_cast<uint8*>(src->imageData + i * ws);
        for (int j = 0, c = nChannel; j < w; ++j, c += n) {
            data[idx++] = ptr[c];
        }
    }
//    printf("?? %d %d\n", src->nChannels, src->widthStep);
//    printf("?? %d %d %d\n", src->width, src->height, idx);

    return idx;
}

bool cvt_YUV2RGB(const uint8 *data_, int w, int h, IplImage *rgb) {
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

bool cvt_RGB2YUV(const IplImage *src, uint8 *data, int nbuf, int *w, int *h) {
    // if data is length is short
    if (src->width * src->height * 3/2 >= nbuf) {
        return false;
    }

    IplImage *yuv_image = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U,
                                        src->nChannels);
    IplImage *comp_yy = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *comp_uu = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    IplImage *comp_vv = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    cvCvtColor(src, yuv_image, CV_BGR2YUV);
    cvSplit(yuv_image, comp_yy, comp_uu, comp_vv, NULL);

    IplImage *comp_u = cvCreateImage(cvSize(src->width / 2, src->height / 2),
                                     IPL_DEPTH_8U, 1);
    IplImage *comp_v = cvCreateImage(cvSize(src->width / 2, src->height / 2),
                                     IPL_DEPTH_8U, 1);
    cvResize(comp_uu, comp_u);
    cvResize(comp_vv, comp_v);

    if (w != NULL) *w = src->width;
    if (h != NULL) *h = src->height;

    int offset = 0;
    offset = get_ipl_data(comp_yy, 0, data);
    offset += get_ipl_data(comp_u, 0, data + offset);
    offset += get_ipl_data(comp_v, 0, data + offset);
    
    cvReleaseImage(&yuv_image);
    cvReleaseImage(&comp_yy);
    cvReleaseImage(&comp_uu);
    cvReleaseImage(&comp_vv);
    cvReleaseImage(&comp_u);
    cvReleaseImage(&comp_v);

    return true;
}

} // namespace vcd
