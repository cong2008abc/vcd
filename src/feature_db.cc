#include "define.h"
#include "global.h"
#include "feature_db.h"
#include "feature/feature.h"
#include "base/vcd_dir.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <vector>
#include <string>

namespace vcd {

typedef unsigned int uint32;
const float OM_THRESHOLD = 0.30;

FeatureDB::FeatureDB() {
    _feat_index = new OMIndex(Global::feature_db_size);
}

FeatureDB::~FeatureDB() {
    delete _feat_index;
}

bool FeatureDB::OpenDB(const char *db_path, int om_type) {
    Dir feat_dir;    
    feat_dir.OpenDir(db_path);
    std::string feature_path;
        int k = 0;
    const OM *pre = NULL;
    int the_same = 0;
    while (true) {
        if (feat_dir.GetNextFile(&feature_path) == false) {
            break;
        }

        FILE *pf = fopen(feature_path.c_str(), "rb");
        if (pf == NULL) {
            printf("Open Feature File Error! %s\n", feature_path.c_str());
            continue;
        } else {
            //printf("%s\n", feature_path.c_str());
        }
        while (true) {
            OM *feat = ReadFeatureFromFile(pf, om_type);
            if (feat == NULL) {
                break;
            }

            k++;
            if (pre != NULL) {
                float tt = pre->Compare(feat);
//                printf("%f\n", tt);
                if (tt == 1.0) {
                    the_same++;
//                    feat->Print();
                    continue;
                } 
            }

            pre = feat;
            if (_feat_index->Insert(feat) == false) {
//                fprintf(stderr, "Error!\n");
            }
            //feat->Print();
//            if (k > 100 * 10000) break;
        }                
        fclose(pf);
    }

    printf("read feature %d %d\n", k, the_same);
    _feat_index->PrintCurrentInfo();

    return true;
}

float FeatureDB::Query(const OM *feat, const OM **ret) {
    return _feat_index->Query(feat, ret);
}


} // namespace vcd
