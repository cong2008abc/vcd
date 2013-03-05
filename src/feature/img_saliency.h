#ifndef _IMG_SALIENCY_H_
#define _IMG_SALIENCY_H_

//#include <pair>
#include <vector>
#include <cv.h>

namespace cv {
    class Mat;
};

// declare the CvRect struct
struct CvRect;

namespace vcd {

typedef std::pair<float, int> CostfIdx;

class Saliency {
public:
    enum SAL_TYPE {
        HC = 0x0,
        RC = 0x1
    };
    // Get saliency map of the giving src img
    static bool Get(const cv::Mat &src, cv::Mat &result, int method = 0);

    // input the saliency map and 
    // return the extract result
    static bool Evaluate(const cv::Mat &src, cv::Mat &result);

    static cv::Mat GetHC(const cv::Mat &img3f);
    static cv::Mat GetRC(const cv::Mat &img3f);
    static cv::Mat GetRC(const cv::Mat&, double, double, int, double);

    static int Quantize(const cv::Mat &img3f, cv::Mat &idx1i, cv::Mat &_color3f,
                        cv::Mat &_color_num, double ratio = 0.95);

    // extract a rectangle area from the input image
    // this rectange contains the main content of the image
    static bool ExtractView(const cv::Mat&, cv::Rect&);

private:
    static const int SAL_TYPE_NUM = 5;
    typedef cv::Mat (*GET_SAL_FUNC)(const cv::Mat&);
    static const GET_SAL_FUNC gfuns[SAL_TYPE_NUM];

    static bool GetHC(const cv::Mat &bin_color3f, const cv::Mat &weights1f,
                      cv::Mat &color_saliency);
    static bool SmoothSaliency(const cv::Mat &bin_color3f, cv::Mat &sal1d,
                               float delta,
                               const std::vector<std::vector<CostfIdx> > &similar);
    static double GetProba(const int *hist, const int kmaxval,
                           int a, int u,
                           double (*fun)(int, int, int));

    static bool ExtractView(const cv::Mat&, cv::Mat&);


    struct Region {
        Region() { pixNum = 0; }
        int pixNum;
        std::vector<CostfIdx> freIdx;
        cv::Point2d centroid;
    };
    static void RegionContrast(const std::vector<Region>&, const cv::Mat&, cv::Mat&, double);
    static void BuildRegions(const cv::Mat&, std::vector<Region>&, const cv::Mat&, int);
};

} // namespace vcd

#endif
