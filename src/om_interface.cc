#include "om_interface.h"
#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "feature/feature.h"
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
//    db = new vcd::FeatureDB();
//    info_db = new vcd::InfoDB();
//
//    db->OpenDB(path);
//    info_db->OpenDB("../info/info.db");

    return 0;
}

int close_db() {
    return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::ImpOMFeature imp;

    imp.ExtractFrame(data, w, h);
    std::list<const vcd::Feature*> r;
    r = db->Query(&imp);

    //show_yuv_colorful(data, w, h);
    std::vector<std::string> result; 
    for(std::list<const vcd::Feature*>::iterator it = r.begin();
        it != r.end(); ++it) {
        const vcd::Feature *ptr = *it;
        std::string path(info_db->GetItem(ptr->GetKeyId()));
        result.push_back(path.substr(0, path.find(".jpg") + 4));
        printf("%d %s %.4f\n",(*it)->GetKeyId(),
                            info_db->GetItem((*it)->GetKeyId()),
                            imp.Compare(ptr));
    }
//    show_query_result(data, w, h, result);

    return 0;
}
