#ifndef _FRAME_INDEX_H_
#define _FRAME_INDEX_H_

#include "feature/feature.h"
#include "frame.h"
#include <map>
#include <vector>
#include <string>

namespace vcd {

enum QUERY_RESULT {
    NO_ONE = 0x1,
    EXIST = 0x2,
    CONFICT = 0x3
};

class FrameIndex {
public:
    FrameIndex();
    virtual ~FrameIndex();

    virtual int ExistFrame(const Frame *ptr);
    virtual int AddFrame(Frame *ptr, int *idx);

private:
    typedef std::map<const uint64, std::vector<Frame*>*> ele_type;
    //typedef std::map<const std::string, std::vector<Frame*>*> n_ele_type;
    ele_type _frame_db;
    //n_ele_type n_frame_db;
};

class FrameIndexS: public FrameIndex {
public:
    FrameIndexS();
    ~FrameIndexS();

    virtual int ExistFrame(const Frame *ptr);
    virtual int AddFrame(Frame *ptr, int *idx);

private:
    typedef std::map<const std::string, std::vector<Frame*>*> s_ele_type;
    s_ele_type _s_frame_db;
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
    typedef std::map<const std::string, std::vector<Frame*>*> ele_type;
    typedef std::map<const std::string, int> ele_count_type;

    ele_type frame_db_;
    ele_count_type frame_count_;

    FrameLRU *frame_lru_;
};

} // namespace vcd

#endif
