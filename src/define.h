#ifndef _DEFINE_H_
#define _DEFINE_H_

#include "feature/define.h"

namespace vcd {

#define ASSGIN(key, frame_id) ((key << 12) | (frame_id))
#define MAIN_KEY(key) (key >> 12)
#define FRAME_ID(key) (key & 0xFFF)

} // namespace vcd

#endif
