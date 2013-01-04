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



/*
 *
 */
FrameLRU::FrameLRU(const int size):
    k_lru_size_(size), pos(0), flag(0) {
    frame_lru_ = new Frame*[k_lru_size_];    
}

FrameLRU::~FrameLRU() {
    delete [] frame_lru_;
}

Frame *FrameLRU::Update(Frame *frame) {
    if (flag == 0) {
        frame_lru_[pos++] = frame;

        if (pos >= k_lru_size_) {
            flag = 1;
            pos = 0;
        }

        return NULL;
    } else {
        Frame *ret = frame_lru_[pos];
        frame_lru_[pos++] = frame; 

        return ret;
    }
}


FrameIndexLRU::FrameIndexLRU(const int size) {
    frame_lru_ = new FrameLRU(size);
}

FrameIndexLRU::~FrameIndexLRU() {
    delete frame_lru_;

    ele_type::iterator it;
    for (it = frame_db_.begin(); it != frame_db_.end(); ++it) {
        std::vector<Frame*> *vt = it->second;
        for (size_t t = 0; t < vt->size(); ++t) {
            delete vt->at(t);
        }
        delete vt;
    }
}

/*
 * insert frame into the framedb
 */
int FrameIndexLRU::Insert(Frame *ptr) {
    const std::string &key = ptr->GetStrKey();
    Frame *del_ptr = NULL;

    // 1= if the key don't appear
    // create a new item
    if (frame_db_.count(key) == 0) {
        std::vector<Frame*> *vt = new std::vector<Frame*>();
        vt->push_back(ptr);
        
        frame_db_[key] = vt;
        frame_count_[key] = 1;
        del_ptr = frame_lru_->Update(ptr);
    } else {
        // 2= check all frame in this item
        // if exist the same frame, do nothing
        std::vector<Frame*> *vt = frame_db_[key];
        size_t t;
        for (t = 0; t < vt->size(); ++t) {
            Frame *cmp_frame = vt->at(t);
            if (cmp_frame->EqualHist(ptr)) {
                break;
            }
        }

        if (t != vt->size()) return 1;

        vt->push_back(ptr);
        frame_count_[key]++;
        del_ptr = frame_lru_->Update(ptr);
    }

    // 3= if the frame db is full
    // delete the old one!
    // 
    // here exist a key-count pair!
    // if count[key] == 0, really delete this frame.
    if (del_ptr != NULL) {
        const std::string &del_key = del_ptr->GetStrKey();

        if (frame_count_[key] == 1) {
            std::vector<Frame*> *vt = frame_db_[del_key];
            for (size_t t = 0; t < vt->size(); ++t) {
                delete vt->at(t);
            }
            delete vt;
            frame_db_.erase(del_key);
        } else {
            frame_count_[key]--;
        }
    }

    return 0;
}

int FrameIndexLRU::Delete(Frame *frame) {}

} // namespace vcd
