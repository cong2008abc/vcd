#ifndef _SIMPLY_FEATURE_INDEX_H_
#define _SIMPLY_FEATURE_INDEX_H_

#include "define.h"

namespace vcd {

typedef bool (*cmp_func)(void *a, void *b);

class SimplyIndex {
public:
    SimplyIndex();
    ~SimplyIndex();

    int Insert(uint64, cmp_func);
    int Insert(uint64);
    int ExistSimilar(uint64, cmp_func);
    bool Clear();

private:
    bool Expend();

private:
    uint64 *arr_;
    uint32 idx_;
    uint32 start_;
    uint32 arr_size_;

    cmp_func default_func_;
};

} // namespace vcd

#endif
