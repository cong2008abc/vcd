#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "feature/define.h"

namespace vcd {

typedef unsigned char   uint8;
typedef unsigned int    uint32;
typedef unsigned long long  uint64;
typedef int             int32;

#define ASSGIN(key, frame_id) ((key << 12) | (frame_id))
#define MAIN_KEY(key) (key >> 12)
#define FRAME_ID(key) (key & 0xFFF)

#define DISALLOW_COPY_AND_ASSIAGN(TypeName) \
private:                                    \
    TypeName(const TypeName &);             \
    TypeName& operator=(const TypeName&)    \

} // namespace vcd

#endif
