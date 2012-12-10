#ifndef _FRAME_H_
#define _FRAME_H_

#include "feature/feature.h"
#include "feature/color_hist.h"

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
    virtual bool EqualOM(const Frame *ptr);
    virtual bool EqualHist(const Frame *ptr);

    bool SetKey(uint32 key);
    uint32 GetKey();

private:
    uint32 _key;

    Feature *_om_dt;        // feature pointer of om feature
    Feature *_color_dt;     // feature pointer of color hist feature
    
    uint64 _hash_key;
};

} // namespace vcd

#endif
