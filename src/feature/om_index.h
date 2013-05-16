#ifndef _OM_INDEX_H_
#define _OM_INDEX_H_
#include "om_feature.h"
#include "common_index.h"

namespace vcd {

class OMIndex {
public:
    OMIndex(int db_num);
    ~OMIndex();

    bool Insert(const OM *ele);
    float Query(const OM *ele, const OM **ret) const;

    bool PrintCurrentInfo();

private:
    int GetHashKey(const OM *ele) const;
    int GetHashKeys(const OM *ele, int *ret) const;
    
private:
    static const int kAvgOfEachDb;

    CommonIndex<const OM*> **_real_db;
    int _db_num;
    int _feat_num;
    float _thres;
    int _speed_thres;

    int _hash_param;

    int _hash_key_num;
    //int* _hash_keys;
};

} // namespace vcd

#endif
