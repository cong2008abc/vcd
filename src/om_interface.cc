#include "om_interface.h"
#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "global.h"
#include "feature/om_feature.h"
#include <list>
#include <vector>
#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <pthread.h>

vcd::FeatureDB *db;
vcd::InfoDB *info_db_query;
vcd::InfoDB *info_db_src;

int open_db(char *path) {
    db = new vcd::FeatureDB();
    db->OpenDB(Global::query_path, Global::query_method);

    info_db_query = new vcd::InfoDB();
    info_db_query->OpenDB(Global::info_db_query);

    info_db_src = new vcd::InfoDB();
    info_db_src->OpenDB(Global::info_db_src);

    return 0;
}

int close_db() {
    delete db;
    return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::OM *feat;
    switch (Global::query_method) {
        case FEAT_OM:
            feat = vcd::SimplyOM::Extract(data, w, h, Global::sub_num);
            break;
        case FEAT_IMP_OM:
            feat = vcd::ImprovedOM::Extract(data, w, h, Global::sub_num);
            break;
        case FEAT_SA_OM:
            feat = vcd::ImprovedOM::ExtractWithSaliency(data, w, h, Global::sub_num);
            break;
        default:
            feat = NULL;
            break;
    }

    if (feat == NULL) {
        return 1;
    }

    const vcd::OM *feat_ret;
    float ret = db->Query(feat, &feat_ret);

    vcd::uint64 id = feat_ret->GetID();
    printf("%f %llu %llu\n", ret, MAIN_KEY(id), FRAME_ID(id));

    delete feat;

    return 0;
}
