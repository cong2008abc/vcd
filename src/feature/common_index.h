/*
 *
 * author: zhouchao(zhouchao@ict.ac.cn)
 *
 * CommonIndex is a linear query index for feature.
 * 
 * the query Function compare each element in the array, 
 * and return the most similar element back.
 *
 * Just use as a secondly store structure when the feature comes large.
 * something like a buck structure in hashtable.
 *
 */

#ifndef _COMMON_INDEX_H_
#define _COMMON_INDEX_H_

namespace vcd {

template<typename T>
class CommonIndex {
    typedef float (*index_cmp)(const T *a, const T *b);
//    typedef float (*index_fast_cmp)(const T *a, const T *b, float thres);

public:
    CommonIndex();
    ~CommonIndex();

    bool Insert(const T *ele);
    const T** GetAllElement(int *ret) const;

    //
    // query the index, and find the most similar one
    //
    // return the similar and the element finded.
    //
    float Query(const T *ele, const T **ret, index_cmp cmp) const;

//    float Query(const T *ele, const T **ret, index_fast_cmp cmp) const;

public:
    bool Expend();

private:
    const T **_arr[2];
    int _brother;
    int _len;
    int _capacity;
};

} // namespace vcd

#endif
