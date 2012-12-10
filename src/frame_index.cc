#include "frame_index.h"

namespace vcd {

FrameIndex::FrameIndex() {

}

FrameIndex::~FrameIndex() {
    ele_type::iterator it; 
    for (it = _frame_db.begin(); it != _frame_db.end(); ++it) {
        std::vector<Frame*> *vt = it->second;
        std::vector<Frame*>::iterator f_it;
        for (f_it = vt->begin(); f_it != vt->end(); ++f_it) {
            delete *f_it;
        }
        delete vt;
    }
}

int FrameIndex::ExistFrame(const Frame *ptr) {
    const uint64 key = ptr->GetHashKey();
    if (_frame_db.find(key) == _frame_db.end()) {
        return 0;
    }

    std::vector<Frame*> *vt = _frame_db[key];
    for(int i = 0; i < vt->size(); ++i) {
        Frame *c_frame = vt->at(i);
        if (c_frame->EqualOM(ptr)) {
            return 1;
        }
    }
    return 0;
}

int FrameIndex::AddFrame(Frame *ptr) {
    if (ExistFrame(ptr) == true) {
        return false;
    }
    const uint64 key = ptr->GetHashKey();
    if (_frame_db.find(key) == _frame_db.end()) {
        std::vector<Frame*> *vt = new std::vector<Frame*>();
        vt->push_back(ptr);
        _frame_db.insert(std::pair<uint32, std::vector<Frame*>*>(key, vt));
    } else {
        _frame_db[key]->push_back(ptr);
    }
    return true;
}

} // namespace vcd
