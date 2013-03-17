#include "vcd_file.h"
#include <string.h>
#include <time.h>

namespace vcd {

const uint32 kCacheSize = 1024 * 32; // 32k cache
// one file store 100M features

File::File(const char *path, const char *type_name,
                 const int max_count)
    : max_size_(max_count), save_size_(0),
      left_space_(kCacheSize), pf_(NULL) {

    cache_ = new char[kCacheSize];
    strcpy(path_, path);
    strcpy(type_name_, type_name);
}

File::~File() {
    if (left_space_ != kCacheSize) {
        Dump();
    }

    if (pf_ != NULL) {
        fclose(pf_);
        pf_ = NULL;
    }

    delete [] cache_;
}

bool File::AppendFrame(Frame *ptr) {
    return false;
}

bool File::Append(void *ptr_, uint32 size) {
	MutexLockGuard lock(locker_);
    // copy the data to the memory cache,
    // if the cache is full, flush it before
    if (left_space_ >= size) {
        CopyToCache(ptr_, size);    
    } else {
        uint32 t = left_space_;
        char *ptr = (char*)ptr_;
        CopyToCache(ptr, left_space_);
        Dump();
        CopyToCache(ptr + t, size - t);
    }

    // add this piece of data to the total size,
    // if this file save enough data, close it!
    // and try to open a new one.
    // in order to control the max size of one vcd file
    save_size_ += size;
    if (save_size_ > max_size_) {
        Dump();
        fclose(pf_);
        pf_ = NULL;
    }
    return true;
}

bool File::Append(const std::string &data) {
    return Append((void*)data.c_str(), data.size());
}

bool File::Append(const char *ptr) {
		uint32 size = strlen(ptr);
		return Append((void*)ptr, size);
}

bool File::CopyToCache(void *ptr, int len) {
    uint32 start_pos = kCacheSize - left_space_;
    memcpy(cache_ + start_pos, ptr, len);
    left_space_ -= len;
    if (left_space_ == 0) {
            Dump();
    }
    return true;
}

bool File::Dump() {
    fprintf(stdout, "Try to Dump FileData!\n");
		
    if (pf_ == NULL) {
        char tmp[128];    
        CreateName(tmp);
        pf_ = fopen(tmp, "w");
    }

    fwrite(cache_, sizeof(char), kCacheSize - left_space_, pf_);
    left_space_ = kCacheSize;

    return true;
}

// vcd file will be named by
// path/[cur_time].[type_name]
bool File::CreateName(char *ret) {
    // 1= get cur time
    time_t cur_time;
    time(&cur_time);
    struct tm *ptime;
    ptime = localtime(&cur_time);

    // 2= assemble all
    sprintf(ret, "%s/%d%.2d%.2d_%.2d%.2d%.2d.%s", path_,
            (1900 + ptime->tm_year), (1 + ptime->tm_mon), 
            ptime->tm_mday, ptime->tm_hour,
            ptime->tm_min, ptime->tm_sec,
            type_name_);

    return true;
}

} // namespace vcd
