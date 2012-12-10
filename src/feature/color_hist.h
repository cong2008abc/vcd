#ifndef _COLOR_HIST_H_
#define _COLOR_HIST_H_

#include "feature.h"

namespace vcd {

class ColorHistFeature: public Feature {
public:
    ColorHistFeature();
    virtual ~ColorHistFeature();

    virtual bool ExtractFrame(const uint8 *data, int, int);
    virtual bool DumpToFile(FILE *pfile);
    virtual bool ReadFromFile(FILE *pfile);
    
    virtual float Compare(const Feature *rf);

    static int FEATURE_LEN;

private:
    float ABS(float x);

private:
    float *_arr_hist;
};

} // namespace vcd

#endif
