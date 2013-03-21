#ifndef _VIDEO_PROCESSOR_H_
#define _VIDEO_PROCESSOR_H_

#include "define.h"

namespace vcd {

typedef int (*process_func)(unsigned char*, int, int);

class VideoProcessor {
public:
    VideoProcessor();
    ~VideoProcessor();

    bool ProcessVideo(const char *path, process_func fun);

private:
    DISALLOW_COPY_AND_ASSIAGN(VideoProcessor); 
    
};
} // namespace vcd

#endif
