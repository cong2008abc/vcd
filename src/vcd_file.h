#ifndef _BIG_FRAME_DB_H_
#define _BIG_FRAME_DB_H_

#include "define.h"
#include "frame.h"
#include <stdio.h>

namespace vcd {

class VcdFile {
public:
    explicit VcdFile(const char *path);
    ~VcdFile();

    bool AppendFrame(Frame *ptr);
private:
    bool Dump();
    bool CopyToCache(const char *ptr, int len);

private:
    char path_[128];
    int frame_count_;
    int cache_len_;
    char *cache_;
    FILE *pf_;
		FILE *hpf_;			// file handle for hist feature
};

} // namespace vcd

#endif
