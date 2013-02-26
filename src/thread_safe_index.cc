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
    const std::string &key = frame->GetOMStr();
    
    int32 ikey = HashFunc(key);
    //printf("Hash Ok! %d %d\n", ikey, kLockersNum);

    // mutex locker
    MutexLock &locker = GetLocker(ikey);
    MutexLockGuard guard(locker);

    fprintf(stderr, "Locker %d Locked!\n", ikey);
    
    //printf("Lock Ok!\n");
    real_idx_ptr_t idx_ptr = GetIndex(ikey);
    int ret = idx_ptr->Insert(frame);

    fprintf(stderr, "Locker %d ", ikey);

    return ret;
}

bool IndexLRU::DeleteFrame(frame_ptr_t &frame) {}

int IndexLRU::HashFunc(const std::string &key) {
    // it comes from the AP hash Function 
    // code copys from the website: www.byvoid.com/blog/string-hash-compare/
    unsigned int hash = 0;
    int i;
    for (i = 0; i < key.size(); ++i) {
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
