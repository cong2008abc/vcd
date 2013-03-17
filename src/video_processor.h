#ifndef _VIDEO_PROCESSOR_H_
#define _VIDEO_PROCESSOR_H_

#include "define.h"

namespace vcd {

class VideoProcessor {
public:
    VideoProcessor();
    ~VideoProcessor();

    bool OpenVideo(const char *path, ...);

private:
    DISALLOW_COPY_AND_ASSIAGN(VideoProcessor); 
};
} // namespace vcd

#endif
