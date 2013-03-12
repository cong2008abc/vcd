#ifndef _IMAGE_BUF_H_
#define _IMAGE_BUF_H_

#include "define.h"
#include "lock.h"

namespace vcd {

struct image_node {
    uint64 om_16i;
    uint32 w;
    uint32 h;
    uint8 data[0];
};

class ImageBuf {
public:
    ImageBuf();
    ~ImageBuf();

    bool AppendImage(const uint8 *data, int w, int h, uint64 feat);
    bool DumpImage(image_node *img);

private:
    uint8 *_buf;
    uint8 *head, *tail;
    uint64 num;
    MutexLock _locker;
    DISALLOW_COPY_AND_ASSIAGN(ImageBuf);
};

} // namespace vcd
#endif
