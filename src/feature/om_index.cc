#include "om_index.h"

namespace vcd {

const int OMIndex::kAvgOfEachDb = 1024 * 4;

float cmp(const OM* &a, const OM* &b) {
    //return a->SpeedCompare(b);
    return a->Compare(b);
}

OMIndex::OMIndex(int db_num): _db_num(db_num), _feat_num(0),
                    _thres(0.0f), _speed_thres(32) {
    if (_db_num <= 0) {
        _db_num = 8196;
    }
    _real_db = new CommonIndex<const OM*>[_db_num];
}

OMIndex::~OMIndex() {
    delete [] _real_db;
}

//
// Like a hashtable, firstly we get a hash position of 
// the ele. and then insert the ele to the selected position.
//
//
bool OMIndex::Insert(const OM *ele) {
    int pos = GetHashKey(ele);

    if (pos < 0 || pos >= _db_num) {
        return false;
    }
    
    if ( _real_db[pos].Insert(ele)) {
        _feat_num++;
        return true;
    } else {
        return false;
    }
}

float OMIndex::Query(const OM *ele, const OM **ret) const {
    int pos = GetHashKey(ele);

    if (pos < 0 || pos >= _db_num) {
        return false;
    }

    return _real_db[pos].Query(ele, ret, cmp);
}

bool OMIndex::PrintCurrentInfo() {
    fprintf(stderr, "Features num: %d\n", _feat_num);
    for (int i = 0; i < _db_num; i++) {
        _real_db[i].Print();
    }
    return true;
}

//
// the real hash function is in the class of ele
// Maybe not a good solution
//
int OMIndex::GetHashKey(const OM *ele) const {
    int key = ele->GetHashKey(this->_hash_param);

    return key % _db_num;
}

} // namespace vcd
