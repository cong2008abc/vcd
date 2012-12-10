#include "define.h"
#include "feature_db.h"
#include "feature/feature.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

namespace vcd {

typedef unsigned int uint32;
const float OM_THRESHOLD = 0.64999;

FeatureDB::FeatureDB() {
    _arr_feat = new Feature*[kMaxDBLen];
    _capacity = kMaxDBLen;
    _cur_len = 0;
}

FeatureDB::~FeatureDB() {
    for (int i = 0; i < _cur_len; ++i) {
        if (_arr_feat[i] != NULL) {
            delete _arr_feat[i];
        }
    }
    delete [] _arr_feat;
}

bool FeatureDB::OpenDB(const char *db_path) {
    struct dirent **namelist;
    int num = scandir(db_path, &namelist, NULL, alphasort);
    if (num <= 0) {
        return false;
    }
    
    for (int i = 0; i < num; ++i) {
        FILE *pf = fopen(namelist[i]->d_name, "r");
        if (pf == NULL) {
            continue;
        }

        /*
         * one feature file may store many features
         */
        while (1) {
            uint32 key_id;
            int ret;
            ret = fread(&key_id, sizeof(uint32), 1, pf);
            if (ret <= 0) break;

            Feature *new_feat = FeatureFactory(kImprovedOM);
            if (new_feat->ReadFromFile(pf) == false) {
                delete new_feat;
                break;
            }
            new_feat->SetKeyId(key_id);
            _arr_feat[_cur_len++] = new_feat;
        }
        fclose(pf);
    }
    
    return true;
}

bool FeatureDB::AddFeature(Feature *feat) {
    if (_cur_len >= _capacity) {
        return false;
    }

    _arr_feat[_cur_len++] = feat;
    return true;
}

/*
 * sort the feature with the same main_key into one feature file
 */
bool FeatureDB::Dump(const char *dir) {
    char tmp[128];
    for (int i = 0; i < _cur_len; ++i) {
        uint32 key_id = _arr_feat[i]->GetKeyId();
        uint32 main_key= MAIN_KEY(key_id);
        sprintf(tmp, "%s/%d.smp", dir, main_key);
        FILE *pf = fopen(tmp, "a+");
        if (pf == NULL) 
            continue;

        fwrite(&main_key, sizeof(uint32), 1, pf);
        _arr_feat[i]->DumpToFile(pf);
        fclose(pf);
    }
    return true;
}

bool FeatureDB::ExistSimilarFeature(const Feature *feat) {
    for (int i = 0; i < _cur_len; ++i) {
        float sim = _arr_feat[i]->Compare(feat);
        if (sim > OM_THRESHOLD) {
            return true;
        }
    }
    return false;
}

} // namespace vcd
