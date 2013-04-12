#ifndef _FEATURE_DB_H
#define _FEATURE_DB_H

#include "feature/feature.h"
#include <list>

namespace vcd {

const int kMaxDBLen = 0xFFFFF;

struct DB_Item {
    Feature *feat;
    char name[64];
};

class FeatureDB {
public:
    FeatureDB();
    ~FeatureDB();

    /*
     * open a db with feature
     */
    bool OpenDB(const char *db_path);

    /*
     * build a db with feature, only read index
     */
    bool BuildDB(const char *db_path);

    /*
     * add feature 
     * give the feature a key_id if need
     */
    bool AddFeature(Feature *feat);

    /*
     * delete feature with key_id
     */
    bool DeleteFeature(int key_id);

    /*
     * dump this db to disk
     */
    bool Dump(const char *dir);

    /*
     *
     */
    int QueryFeature(const Feature *feat);

    bool ExistSimilarFeature(const Feature *feat);

    std::list<const Feature*> Query(const Feature *feat);

    const Feature* GetFeature(int idx);

private:
    Feature **_arr_feat;
    int _capacity;
    int _cur_len;
    

    DB_Item *item;

    //char db_path[256];
};

} // namespace vcd

#endif
