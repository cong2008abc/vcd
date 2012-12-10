#ifndef _DATA_STREAM_H_
#define _DATA_STREAM_H_

//#include "storage/base.h"
#include "feature_db.h"
#include "info_db.h"
#include <stdio.h>

namespace vcd {


class DataStream {
public:
    DataStream();
    ~DataStream();

    //virtual Open(const char *path) = 0;
    //virtual Close() = 0;

    bool OpenVideo(FILE *pf);
    bool OpenJpg(FILE *pf);
    bool OpenYuv(FILE *pf);

    bool OpenJpg(const char *jpg_name);

    bool DetectMedia(const char *path);
    bool BuildMediaLib(const char *path);

private:
    //struct dirent **_namelist;
    FeatureDB *_feat_db;
    InfoDB *_info_db;
    uint8 *_img_data;
};

} // namespace vcd
#endif
