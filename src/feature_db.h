#ifndef _FEATURE_DB_H
#define _FEATURE_DB_H

#include "feature/feature.h"

namespace vcd {

const int kMaxDBLen = 0xFFFFF;

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

private:
    Feature **_arr_feat;
    int _capacity;
    int _cur_len;

    //char db_path[256];
};

} // namespace vcd

#endif
