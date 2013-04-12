#include "simply_feature_index.h"
#include <stdio.h>

namespace vcd {

/*
 * default compare func for simply_index
 */
bool default_cmp(void *a, void *b) {
    uint64 ra = *(uint64*)a;
    uint64 rb = *(uint64*)b;

    uint64 diff = ra ^ rb;

    int count = 0;
    while (diff != 0) {
        diff &= (diff - 1);
        count++;
    }

    //printf("diff : %d %llu\n", count, rb);
    if (count > 12) return false;
    return true;
}

SimplyIndex::SimplyIndex()
    :idx_(1), start_(1), arr_size_(1024 * 10) {
    arr_ = new uint64[arr_size_];
    default_func_ = default_cmp;
}

SimplyIndex::~SimplyIndex() {
    delete [] arr_;
}

int SimplyIndex::Insert(uint64 feat, cmp_func func) {
    int query = ExistSimilar(feat, func);
    //printf("?? %d\n", query);
    if (query != -1) {
        return -1 * query;
    }
    
    if (arr_size_ == idx_) {
        if (Expend() == false)
            return -1;
    }

    arr_[idx_++] = feat;

    return idx_ - 1;
}

int SimplyIndex::Insert(uint64 feat) {
    return Insert(feat, default_func_); 
}

int SimplyIndex::ExistSimilar(uint64 feat, cmp_func func) {
    for (uint32 i = start_; i < idx_; ++i) {
        if ((*func)(&feat, &arr_[i]) == true)
            return i;
    }
    return -1;
}

bool SimplyIndex::Clear() {
    idx_ = 0;

    return true;
}

bool SimplyIndex::Expend() {
    uint64 *n_arr;
    try {
        n_arr = new uint64[arr_size_ * 2];
    } catch (...) {
        return false;
    }

    for (uint32 i = 0; i < arr_size_; i++) {
        n_arr[i] = arr_[i];
    }

    delete [] arr_;
    arr_ = n_arr;

    return true;
}

} 
