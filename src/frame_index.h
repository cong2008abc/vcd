#ifndef _FRAME_INDEX_H_
#define _FRAME_INDEX_H_

#include "feature/feature.h"
#include "frame.h"
#include <map>
#include <vector>

namespace vcd {

class FrameIndex {
public:
    FrameIndex();
    ~FrameIndex();

    int ExistFrame(const Frame *ptr);
    int AddFrame(Frame *ptr);

private:
    typedef std::map<const uint64, std::vector<Frame*>*> ele_type;
    ele_type _frame_db;
};

} // namespace vcd

#endif
