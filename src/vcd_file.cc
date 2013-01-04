#include "vcd_file.h"
#include <string.h>
#include <time.h>

namespace vcd {

const int kCacheSize = 1024 * 32; // 4M cache
// one file store 100M features
const int kMaxFrameCount = 1024 * 1024 * 100; 

VcdFile::VcdFile(const char *path) {
    pf_ = NULL;
    frame_count_ = 0;
    cache_len_ = 0;

    cache_ = new char[kCacheSize];
    strcpy(path_, path);
}

VcdFile::~VcdFile() {
    if (cache_len_ != 0) {
        Dump();
    }

    if (pf_ != NULL) {
        fclose(pf_);
        pf_ = NULL;
    }

    delete [] cache_;
}

bool VcdFile::AppendFrame(Frame *ptr) {
    const std::string &key = ptr->GetStrKey();
    //printf("%s", key.c_str());

    size_t t = key.size();
    
    if (cache_len_ + t < kCacheSize) {
        CopyToCache(key.c_str(), t);
    } else {
        size_t r = kCacheSize - cache_len_;
        CopyToCache(key.c_str(), r);
        Dump();
        if (r != t) {
            CopyToCache(key.c_str() + r, t - r);
        }
    }

    //printf("%d %d %s\n", cache_len_, t, key.c_str());

    // copy the [ str + '\n' ] to the cache

    frame_count_++;
    if (frame_count_ >= kMaxFrameCount) {
        Dump();
        fclose(pf_);
        pf_ == NULL;
    }

    return true;
}

bool VcdFile::CopyToCache(const char *ptr, int len) {
    strncpy(cache_ + cache_len_, ptr, len);
    cache_len_ += len;
}

bool VcdFile::Dump() {
    fprintf(stdout, "Try to Dump Frame!\n");

    if (pf_ == NULL) {
        char tmp[128];    
        time_t cur_time;
        time(&cur_time);

        struct tm *ptime;
        ptime = localtime(&cur_time);

        sprintf(tmp, "%s/%d%d%d_%d%d%d.db", path_,
                (1900 + ptime->tm_year), (1 + ptime->tm_mon), 
                ptime->tm_mday, ptime->tm_hour,
                ptime->tm_min, ptime->tm_sec);
        
        pf_ = fopen(tmp, "w");
        if (pf_ == NULL) {
            fprintf(stderr, "Open db File Error!\n");
            return false;
        }
    }

    fwrite(cache_, sizeof(char), cache_len_, pf_);
    cache_len_ = 0;

    return true;
}

} // namespace vcd
