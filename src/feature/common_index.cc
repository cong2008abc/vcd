#include "common_index.h"
#include <stdio.h>

namespace vcd {

template<typename T>
CommonIndex<T>::CommonIndex(): _brother(0), _len(0), _capacity(16) {
    _arr[0] = new const T*[_capacity];
}

template<typename T>
CommonIndex<T>::~CommonIndex() {
    for (int i = 0; i < _len; i++) {
        delete _arr[_brother][i];
    }
    delete [] _arr[_brother];
}

template<typename T>
bool CommonIndex<T>::Insert(const T *ele) {
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
float CommonIndex<T>::Query(const T *ele, const T **ret, index_cmp cmp) const {
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
const T** CommonIndex<T>::GetAllElement(int *len) const {
    *len = _len;
    return _arr;
}

template<typename T>
bool CommonIndex<T>::Expend() {
    // try to alloc a new memory 
    // return false if failure
    //
    const T** temp;
    try {
        temp = new const T*[2 * _capacity];
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

    return true;
}
 

} // namespace vcd
