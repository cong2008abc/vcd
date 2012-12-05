#include "define.h"
#include "data_stream.h"
#include "feature/feature.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

namespace vcd {

#define F_DB_PATH "../feature_db"
#define INFO_DB_PATH "../info.db"

typedef unsigned char uint8;

DataStream::DataStream() {
    _feat_db = new FeatureDB();
    _info_db = new InfoDB();

    //_feat_db->BuildDB(F_DB_PATH);
    _info_db->OpenDB(INFO_DB_PATH);
}

DataStream::~DataStream() {
    _feat_db->Dump(F_DB_PATH);
    _info_db->Dump(INFO_DB_PATH);
}

bool DataStream::BuildMediaLib(const char *path) {
    struct dirent **namelist;
    int file_num = scandir(path, &namelist, NULL, alphasort);
    if (file_num <= 0) {
        fprintf(stderr, "Open File Dir Error!\n");
        return false;
    }
    
    //_feat_db = new FeatureDB(DB_PATH);
    for (int i = 0; i < file_num && i < 10; ++i) {
        char full_name[128];
        const char *media_name = namelist[i]->d_name;
        sprintf(full_name, "%s%s", path, media_name);
        printf("%s\n", full_name);

        // here can be imprvoed.
        OpenJpg(full_name);
    }

    return true;
}

bool DataStream::OpenJpg(const char *jpg_name) {
//    FILE *pf = fopen(jpg_name, "r");
//    if (pf == NULL) return false;
    // here is a un-smart function.
    // load_jpg_image will alloc memory and return it,
    // because i don't know how many memory i need for this jpg
    int w, h;
    uint8 *data = load_jpg_image(jpg_name, w, h);

    // detect this feature
    Feature *feat = FeatureFactory(kImprovedOM);
    feat->ExtractFrame(data, w, h);

    // get a key_id of this jpg
    uint32 base_id = _info_db->Insert(jpg_name);
    uint32 full_key = ASSGIN(base_id, 0x1);
    feat->SetKeyId(full_key);

    // add this feature to the library
    _feat_db->AddFeature(feat);

    delete [] data;

    return true;
}

} // namespace vcd
