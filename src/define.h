#ifndef _DEFINE_H_
#define _DEFINE_H_

//#include "feature/define.h"

namespace vcd {

typedef unsigned char   uint8;
typedef unsigned int    uint32;
typedef unsigned long long  uint64;
typedef int             int32;

#define ASSGIN(key, frame_id) ((key << 16) | (frame_id))
#define MAIN_KEY(key) (key >> 16)
#define FRAME_ID(key) (key & 0xFFFF)

#define DISALLOW_COPY_AND_ASSIAGN(TypeName) \
private:                                    \
    TypeName(const TypeName &);             \
    TypeName& operator=(const TypeName&)    \

} // namespace vcd

#define _VCD_DEBUG

enum METHOD {
    FEAT_OM = 0x1,
    FEAT_IMP_OM = 0x2,
    FEAT_SA_OM = 0x4,
};

#endif
