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
    typedef float (*index_cmp)(T &a, T &b);
//    typedef float (*index_fast_cmp)(const T *a, const T *b, float thres);

public:
    CommonIndex();
    ~CommonIndex();

    bool Insert(T &ele);
    const T* GetAllElement(int *ret) const;

    //
    // query the index, and find the most similar one
    //
    // return the similar and the element finded.
    //
    float Query(T &ele, T *ret, index_cmp cmp) const;

    bool Print();

//    float Query(const T *ele, const T **ret, index_fast_cmp cmp) const;

public:
    bool Expend();

private:
    T *_arr[2];
    int _brother;
    int _len;
    int _capacity;
};

template<typename T>
CommonIndex<T>::CommonIndex(): _brother(0), _len(0), _capacity(16) {
    _arr[0] = new T[_capacity];
}

template<typename T>
CommonIndex<T>::~CommonIndex() {
    for (int i = 0; i < _len; i++) {
        delete _arr[_brother][i];
    }
    delete [] _arr[_brother];
}

template<typename T>
bool CommonIndex<T>::Insert(T &ele) {
    //
    // if the current array is full, we need expend it
    //
    if (_len == _capacity) {
        if (Expend() == false) {
            return false;
        }
    }

    _arr[_brother][_len++] = ele;

    return true;
}

template<typename T>
float CommonIndex<T>::Query(T &ele, T *ret, index_cmp cmp) const {
    float vmax = -1.0;
    int pos = -1;
    //
    // use the functon compare each element in the array
    //
    for (int i = 0; i < _len; i++) {
        float ret = (*cmp)(_arr[_brother][i], ele);
        if (ret > vmax) {
            vmax = ret;
            pos = i;
        }
    }

    if (pos == -1) {
        *ret = NULL;
        return -1.0;
    } else {
        *ret = _arr[_brother][pos];
        return vmax;
    }
}

template<typename T>
const T* CommonIndex<T>::GetAllElement(int *len) const {
    *len = _len;
    return _arr;
}

template<typename T>
bool CommonIndex<T>::Print() {
    fprintf(stderr, "feat: %d, capacity: %d\n", _len, _capacity);
    return true;
}

template<typename T>
bool CommonIndex<T>::Expend() {
    // try to alloc a new memory 
    // return false if failure
    //
    T* temp;
    try {
        temp = new T[_capacity << 1];
    } catch (...) {
        temp = NULL;
    }

    if (temp == NULL) {
        return false;
    }


    // copy the data from the memory allocated before
    for (int i = 0; i < _len; i++) {
        temp[i] = _arr[_brother][i];
    }

    // update the memory we allocated to the last one
    // and delete the old one
    //
    delete [] _arr[_brother];
    _arr[_brother] = temp;
    _capacity = _capacity << 1;

    return true;
}
 

} // namespace vcd

#endif
