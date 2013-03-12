#ifndef _FRAME_H_
#define _FRAME_H_

#include "feature/feature.h"
#include "feature/color_hist.h"
#include <string>

namespace vcd {

typedef unsigned int uint32;
typedef unsigned long long uint64;

class Frame {
    //friend class FrameIndex;
public:
    Frame();
    virtual ~Frame();

    virtual bool ExtractFeature(const uint8 *data, int, int);
    virtual uint64 GetHashKey() const;
    virtual const std::string& GetOMStr() const;
    virtual bool EqualOM(const Frame *ptr);
    virtual bool EqualHist(const Frame *ptr);

    virtual bool DumpToFile(FILE *pf);

    bool SetKey(uint32 key);
    uint32 GetKey();

private:
    uint32 _key;

//    Feature *_om_dt;        // feature pointer of om feature
//    Feature *_color_dt;     // feature pointer of color hist feature
    ImpOMFeature *_om_dt;
    ColorHistFeature *_color_dt;
    
    uint64 _hash_key;
    std::string _str_hash_key;
};

} // namespace vcd

#endif
