#include "frame_index.h"
#include <iostream>

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

int FrameIndex::AddFrame(Frame *ptr, int *idx) {
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





/*
 * implement of FrameIndexS
 */
FrameIndexS::FrameIndexS() {

}

FrameIndexS::~FrameIndexS() {
    s_ele_type::iterator it; 
    for (it = _s_frame_db.begin(); it != _s_frame_db.end(); ++it) {
        std::vector<Frame*> *vt = it->second;
        std::vector<Frame*>::iterator f_it;
        for (f_it = vt->begin(); f_it != vt->end(); ++f_it) {
            delete *f_it;
        }
        delete vt;
    }
}

int FrameIndexS::ExistFrame(const Frame *ptr) {
    const std::string &key = ptr->GetStrKey();
    if (_s_frame_db.find(key) == _s_frame_db.end()) {
        return NO_ONE;
    }

    //return CONFICT;

    std::vector<Frame*> *vt = _s_frame_db[key];
    for (int i = 0; i < vt->size(); ++i) {
        Frame *p_frame = vt->at(i);
        if (p_frame->EqualHist(ptr)) {
            return EXIST;
        }
    }
    return CONFICT;
}

int FrameIndexS::AddFrame(Frame *ptr, int *idx) {
    int query_ret = ExistFrame(ptr);

    if (query_ret == NO_ONE) {
        std::vector<Frame*> *vt = new std::vector<Frame*>();
        vt->push_back(ptr);
        _s_frame_db.insert(std::pair<const std::string, std::vector<Frame*>*>(ptr->GetStrKey(), vt));
        *idx = 0;
    } else if (query_ret == EXIST) {
        *idx = -1;
    } else if (query_ret == CONFICT) {
        *idx = _s_frame_db[ptr->GetStrKey()]->size();
        _s_frame_db[ptr->GetStrKey()]->push_back(ptr);
    } else {
        ;
    }

    return query_ret;
}

} // namespace vcd
