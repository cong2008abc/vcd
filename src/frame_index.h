#ifndef _FRAME_INDEX_H_
#define _FRAME_INDEX_H_

#include "feature/feature.h"
#include "frame.h"
#include <map>
#include <vector>
#include <string>

namespace vcd {

//enum QUERY_RESULT {
//    NO_ONE = 0x1,
//    EXIST = 0x2,
//    CONFICT = 0x3
//};

enum INSERT_RESULT {
    EXIST = 0x1,
    NEW = 0x2,
    REPEAT = 0x3
};

/*
 *
 */

class FrameLRU {
public:
    FrameLRU(const int size);
    ~FrameLRU();
    
    Frame* Update(Frame *frame);

private:
    Frame **frame_lru_;
    const int k_lru_size_;
    int pos;
    int flag;
};

const int kDefaultFrameCount = 1024 * 1024 * 1;

class FrameIndexLRU {
public:
    FrameIndexLRU(const int size = kDefaultFrameCount);
    ~FrameIndexLRU();

    virtual int Insert(Frame *ptr);
    virtual int Delete(Frame *ptr);

private:
    typedef const uint64 key_type;
    typedef std::vector<Frame*> val_type;
    typedef std::map<key_type, val_type*> ele_type;
    typedef std::map<key_type, int> ele_count_type;

    ele_type frame_db_;
    ele_count_type frame_count_;

    FrameLRU *frame_lru_;
};

} // namespace vcd

#endif
