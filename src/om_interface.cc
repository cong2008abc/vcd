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
vcd::InfoDB *info_db;

int open_db(char *path) {
    db = new vcd::FeatureDB();
    db->OpenDB(Global::db_path, Global::query_method);

    return 0;
}

int close_db() {
    delete db;
    return 0;
}

int query_image(unsigned char *data, int w, int h) {

    switch (Global::query_method) {
        case FEAT_OM:
            vcd::OM *feat = vcd::SimplyOM::Extract(data, w, h, Global::sub_num);
            const vcd::OM *feat_ret;
            float ret = db->Query(feat, &feat_ret);

            vcd::uint64 id = feat_ret->GetID();
            printf("%f %llu %llu\n", ret, MAIN_KEY(id), FRAME_ID(id));
            break;
    }

    return 0;
}
