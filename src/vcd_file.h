#ifndef _BIG_FRAME_DB_H_
#define _BIG_FRAME_DB_H_

#include "define.h"
#include "frame.h"
#include "lock.h"
#include <stdio.h>

namespace vcd {

// max size of one vcf log file 2G
const uint64 kMaxSize = 1024 * 1024 * 1024 * 2u; 

class File {
public:
    explicit File(const char *path, const char *type_name,
                  const int max_count = kMaxSize);
    ~File();

    bool AppendFrame(Frame *ptr);
    bool Append(void *ptr, uint32 size);
    bool Append(const std::string &data);
    bool Append(const char *ptr);
private:
    bool Dump();
    bool CopyToCache(void *ptr, int len);
    bool CreateName(char *ret);

private:
    char path_[128];
    char type_name_[16];

    uint64 max_size_;      // the max size of one vcd file
    uint32 save_size_;     // the size of data has writed into disk
    uint32 left_space_;    // the size of data in the memory
    char *cache_;
    FILE *pf_;

    MutexLock locker_;
};

} // namespace vcd

#endif
