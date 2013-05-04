#ifndef _FEATURE_DB_H
#define _FEATURE_DB_H

#include "feature/feature.h"
#include "feature/om_feature.h"
#include "feature/om_index.h"
#include <list>

namespace vcd {

class FeatureDB {
public:
    FeatureDB();
    ~FeatureDB();

    /*
     * open a db with feature
     */
    bool OpenDB(const char *db_path);
    bool OpenDB(const char *db_path, int type);

    float Query(const OM *feat,  const OM **ret);

private:
    OMIndex *_feat_index;
};

} // namespace vcd

#endif
