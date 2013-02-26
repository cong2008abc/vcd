#ifndef _SALIENCY_H_
#define _SALIENCY_H_

#include "define.h"

namespace vcd {

class Saliency {
    struct Point {
        uint32 x;
        uint32 y;

        Point(uint32 i, uint32 j) {
            x = i;
            y = j;
        }
    };

public:
    Saliency();
    ~Saliency();

    /*
     * input the image and return the saliency map of the input
     */
    bool ExtractSaliency(const uint8 *data, const int w, const int h, uint8 *result);
    bool ExtractSaliency2(const uint8 *data, const int w,
                          const int h, uint8 *result);
    bool ExtractSaliency3(const uint8 *data, const int w,
                          const int h, uint8 *result);
    bool ExtractSaliencyWithColor(const uint8 *data, const int w,
                          const int h, uint8 *result);

    /* 
     * quantized blocked image 
     */
    bool QuantizedBlockedImage(const uint8 *data, const int w,
                               const int h, uint8 *result);

    /*
     * input the image and
     * return the binary map of the input
     */
    bool ExtractArea(const uint8 *data, const int w, const int h, uint8 *result);

private:
    uint8 GetDistance(uint8 a, uint8 b);

    bool IsMaxVal(const uint8 *data, const int w, const int cw, const int ch, const int size);

    bool ExtendArea(const uint8 *data, const int w, const int h,
                    const Point &center,
                    int *left, int *top, int *area_w, int *area_h);

    bool IfExtendRow(const uint8 *data, const int w, const int h,
                     const int left, const int right,
                     const int, const int);
    bool IfExtendCol(const uint8 *data, const int w, const int h,
                     const int top, const int bottom,
                     const int, const int);

};

} // namespace vcd

#endif
