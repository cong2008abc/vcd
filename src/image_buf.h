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
    const uint8* GetData();
    bool DumpInfo();

private:
    bool RemoveImage();

private:
    uint8 *_buf[2];
    uint8 *_real_buf;
    uint8 *_end, *_tail;
//    uint64 _num, _left;
    MutexLock _locker;
    DISALLOW_COPY_AND_ASSIAGN(ImageBuf);
};

ImageBuf* image_buffer_init(int buf_num_of_file, int max_tmp_num,
                            const char *db_path);
void insert_repeat_feature(uint64 om_16i);
void close_image_buffer();

} // namespace vcd
#endif
