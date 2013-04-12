#include "define.h"
#include "feature_db.h"
#include "feature/feature.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <algorithm>
#include <vector>

namespace vcd {

typedef unsigned int uint32;
const float OM_THRESHOLD = 0.45;

FeatureDB::FeatureDB() {
    _arr_feat = new Feature*[kMaxDBLen];
    _capacity = kMaxDBLen;
    _cur_len = 0;

    //item = new DB_Item[kMaxDBLen];
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
    
    //FILE *merge_pf = fopen("../feature_db2/1.smp", "wb");
    //char buf[1024 * 10];
    for (int i = 0; i < num; ++i) {
        char name[64];
        sprintf(name, "%s/%s", db_path, namelist[i]->d_name);
        if (i % 100 == 0)
            printf("%d\n", i);
        FILE *pf = fopen(name, "rb");
        if (pf == NULL) {
            continue;
        }

        //int len = fread(buf, sizeof(char), 1024, pf);
        //fwrite(buf, sizeof(char), len, merge_pf);

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
            //if (_cur_len == 1) _arr_feat[0]->print();
        }
        fclose(pf);
        free(namelist[i]);
    }

    //fclose(merge_pf);

    printf("%d\n", _cur_len);
    
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
    printf("--exist %d features\n", _cur_len);
    for (int i = 0; i < _cur_len; ++i) {
        uint32 key_id = _arr_feat[i]->GetKeyId();
        //printf("%d\n", key_id);
        uint32 main_key = key_id;
        //uint32 main_key= MAIN_KEY(key_id);
        sprintf(tmp, "%s/%d.smp", dir, main_key);
        FILE *pf = fopen(tmp, "wb");
        if (pf == NULL) { 
            printf("failed when %s\n", tmp);
            continue;
        }

        fwrite(&main_key, sizeof(uint32), 1, pf);
        _arr_feat[i]->DumpToFile(pf);
        fclose(pf);

        if (i == 0) _arr_feat[i]->print();
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

struct result_tmp {
    float sim;
    const Feature *ptr;

    result_tmp(float s, const Feature *p) {
        sim = s;
        ptr = p;
    }
};

bool cmp(const result_tmp &a, const result_tmp &b) {
    return a.sim > b.sim;
}

std::list<const Feature*> FeatureDB::Query(const Feature *feat) {
    std::list<const Feature*> result;
    std::vector<result_tmp> tmp;
    for (int i = 0; i < _cur_len; ++i) {
        float sim = _arr_feat[i]->Compare(feat);
        //printf("%f\n", sim);
        if (sim > OM_THRESHOLD) {
            //printf("get result: %d\n", _arr_feat[i]->GetKeyId());
            //result.push_back(_arr_feat[i]);
            tmp.push_back(result_tmp(sim, _arr_feat[i]));
        }
    }
    std::sort(tmp.begin(), tmp.end(), cmp);
    for (int i = 0; i < 9 && i < tmp.size(); ++i) {
        result.push_back(tmp[i].ptr);
    }
    return result;
}

} // namespace vcd
