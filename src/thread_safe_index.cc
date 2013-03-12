#include "thread_safe_index.h"

namespace vcd {

IndexLRU::IndexLRU(const int size) {
    int each_size_of_idx = size / kLockersNum;
    for (int i = 0; i <= kLockersNum; ++i) {
        frame_idx_ptr_[i] = real_idx_ptr_t(new FrameIndexLRU(each_size_of_idx));
    }
}

IndexLRU::~IndexLRU() {}

bool IndexLRU::InsertThreadSafe(frame_ptr_t &frame) {
//    const std::string &key = frame->GetOMStr();
    
    uint64 key_ = frame->GetComprsFeature();
    uint8 *key = reinterpret_cast<uint8*>(&key_);
    int32 ikey = HashFunc(key, 8);

    // mutex locker
    MutexLock &locker = GetLocker(ikey);
    MutexLockGuard guard(locker);

    real_idx_ptr_t idx_ptr = GetIndex(ikey);
    int ret = idx_ptr->Insert(frame);

    return ret;
}

bool IndexLRU::DeleteFrame(frame_ptr_t &frame) {
    return false;
}

int IndexLRU::HashFunc(uint8 *key, int len) {
    // it comes from the AP hash Function 
    // code copys from the website: www.byvoid.com/blog/string-hash-compare/
    unsigned int hash = 0;
    int32 i;
    for (i = 0; i < len; ++i) {
        if ((i & 1) == 0) {
            hash ^= ((hash << 7) ^ (key[i]) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (key[i]) ^ (hash >> 5)));
        }
    }

    return (hash & kLockersNum);
}

MutexLock& IndexLRU::GetLocker(int32 ikey) {
    return mutex_lock_[ikey];
}

IndexLRU::real_idx_ptr_t IndexLRU::GetIndex(int32 ikey) {
    return frame_idx_ptr_[ikey];
}

} // namespace vcd
