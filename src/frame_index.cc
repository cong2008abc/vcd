#include "frame_index.h"
#include <iostream>

namespace vcd {

/*
 * class FrameLRU is a ring struct store the frame ptr
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

				if (pos >= k_lru_size_) {
						pos = 0;
				}

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
//    const std::string &key = ptr->GetOMStr();
    const key_type key = ptr->GetComprsFeature();
    Frame *del_ptr = NULL;
    int status = -1;

    // 1= if the key don't appear
    // create a new item
    if (frame_db_.find(key) == frame_db_.end()) {
        val_type *vt = new val_type();
        vt->push_back(ptr);
        
        frame_db_[key] = vt;
        frame_count_[key] = 1;
        del_ptr = frame_lru_->Update(ptr);

        status = NEW;
//        fprintf(stderr, "Create Item\n");
    } else {
        // 2= check all frame in this item
        // if exist the same frame, do nothing
//		fprintf(stderr, "Insert %s\n", key.c_str());
//		fflush(stderr);
        val_type *vt = frame_db_[key];
        size_t t;
        for (t = 0; t < vt->size(); ++t) {
            Frame *cmp_frame = vt->at(t);
            if (cmp_frame->EqualHist(ptr)) {
                break;
            }
        }

//        if (t != vt->size()) {
//            return EXIST;
//        }

        vt->push_back(ptr);
        frame_count_[key]++;
        del_ptr = frame_lru_->Update(ptr);
        status = REPEAT;
    }

    // 3= if the frame db is full
    // delete the old one!
    // 
    // here exist a key-count pair!
    // if count[key] == 0, really delete this frame.
    if (del_ptr != NULL) {
//        const std::string del_key = del_ptr->GetOMStr();
        const key_type del_key = del_ptr->GetComprsFeature();

        if (frame_count_[del_key] == 1) {
            val_type *vt = frame_db_[del_key];
            for (size_t t = 0; t < vt->size(); ++t) {
                delete vt->at(t);
            }
            delete vt;
            frame_db_.erase(del_key);
						//fprintf(stderr, "Find Key %s = %d\n", del_key.c_str(),
						//				frame_db_.find(del_key) == frame_db_.end());
        } else {
            frame_count_[del_key]--;
        }
    }

    return status;
}

int FrameIndexLRU::Delete(Frame *frame) {
    return 1;
}

} // namespace vcd
