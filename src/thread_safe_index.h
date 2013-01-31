#ifndef _THREAD_SAFE_INDEX_H_
#define _THREAD_SAFE_INDEX_H_

#include "define.h"
#include "lock.h"
#include "frame.h"
#include "frame_index.h"
#include <tr1/memory>
#include <string>
#include <vector>
#include <map>

namespace vcd {

const int kLockersNum = 0x7;
//const int kMaxFrameNum = 1024 * 1024 * 1;
const int kMaxFrameNum = 1024u * 10;

class IndexLRU {
    typedef Frame* frame_ptr_t;
    typedef std::tr1::shared_ptr<FrameIndexLRU> real_idx_ptr_t;
public:
    IndexLRU(const int size = kMaxFrameNum);
    ~IndexLRU();

    bool InsertThreadSafe(frame_ptr_t &frame);
    bool DeleteFrame(frame_ptr_t &frame);

private:
    MutexLock& GetLocker(int32 int_key);
    real_idx_ptr_t GetIndex(int32 int_key);
    int HashFunc(const std::string &key);
    
    //frame_ptr_t& GetPtrLru(frame_ptr_t &frame);

private:
    MutexLock mutex_lock_[kLockersNum + 1];
    real_idx_ptr_t frame_idx_ptr_[kLockersNum + 1];
};

} // namespace vcd

#endif
