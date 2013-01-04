#include "utils.h"
#include "sa_cvpr_09.h"

namespace vcd {

const float PI = 3.14159;

bool filter_gauss(IplImage *src, IplImage *dst) {
    cvSmooth(src, dst, CV_GAUSSIAN, 3);
}

bool cvpr09_saliency(IplImage *src) {
    IplImage *gray = get_gray(src);
    IplImage *dst = cvCloneImage(gray);
    //filter_gauss(src, dst);
    cvSmooth(gray, dst, CV_GAUSSIAN, 0, 0, PI);

    CvScalar avg = cvAvg(dst);
    //int avg_val = avg.val[0];
    cvSubS(dst, avg, dst);
    show_image(dst, "cvpr09");
}

} // namespace vcd
