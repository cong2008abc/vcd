#include "om_index.h"

namespace vcd {

const int OMIndex::kAvgOfEachDb = 1024 * 4;

float cmp(const OM* &a, const OM* &b) {
    return a->SpeedCompare(b, 23);
    //return a->Compare(b);
}

OMIndex::OMIndex(int db_num): _db_num(db_num), _feat_num(0),
                              _thres(0.0f), _speed_thres(32),
                              _hash_param(9),
                              _hash_key_num(1) {
    if (_db_num <= 0) {
        _db_num = 8196;
    }
    _real_db = new CommonIndex<const OM*>*[_hash_key_num];
    for (int i = 0; i < _hash_key_num; i++) {
        _real_db[i] = new CommonIndex<const OM*>[_db_num];
    }
    //_hash_keys = new int[_hash_key_num];
}

OMIndex::~OMIndex() {
    for (int i = 0; i < _hash_key_num; i++)
        delete [] _real_db[i];
    delete [] _real_db;
}

//
// Like a hashtable, firstly we get a hash position of 
// the ele. and then insert the ele to the selected position.
//
//
bool OMIndex::Insert(const OM *ele) {
//    int pos = GetHashKey(ele);
//
//    if (pos < 0 || pos >= _db_num) {
//        printf("??Er %d %d\n", pos, _db_num);
//        return false;
//    }
    
//    int *pos = new int[_hash_key_num];
    static int pos[64];

    if (GetHashKeys(ele, pos) != 0) {
        printf("Insert Er!\n");
        goto Failure;
    }

    for (int i = 0; i < _hash_key_num; i++) {
        if (_real_db[i][pos[i]].Insert(ele) == false) {
            goto Failure; 
        }
        _feat_num++;
    }

    return true;

Failure:
    return false;
}

float OMIndex::Query(const OM *ele, const OM **ret) const {
    //int *pos = new int[_hash_key_num];
    static int pos[64];
    if (GetHashKeys(ele, pos) != 0) {
        printf("Er!\n");
        delete [] pos;
        return -1.0;
    }

    float vmax = -1.0;
    const OM *max_ret;
    for (int i = 0; i < _hash_key_num; i++) {
        //printf(".. query hash_key : %d\n", i);  
        float tmp = _real_db[i][pos[i]].Query(ele, &max_ret, cmp); 
        if (tmp > vmax) {
            *ret = max_ret;
            vmax = tmp;
        }
    }

    return vmax;
}

bool OMIndex::PrintCurrentInfo() {
    fprintf(stderr, "Features num: %d\n", _feat_num);
    for (int id = 0; id < _hash_key_num; id++) {
        for (int i = 0; i < _db_num; i++) {
            printf("hash item info[%d]: ", i);
            _real_db[id][i].Print();
        }
    }
    return true;
}

//
// the real hash function is in the class of ele
// Maybe not a good solution
//
int OMIndex::GetHashKey(const OM *ele) const {
    uint64 key = ele->GetHashKey(this->_hash_param);

    return key % _db_num;
}

int OMIndex::GetHashKeys(const OM *ele, int *_ret) const {
    static uint64 ret[64];
    //printf(".. getting hash key!\n");
    if (ele->GetHashKeys(_hash_param, ret, _hash_key_num) == false) {
        return 1; 
    }

    for (int i = 0; i < _hash_key_num; i++) {
        //printf(".. %llu\n", ret[i]);
        _ret[i] = ret[i] % _db_num;
    }

    return 0;
}

} // namespace vcd
