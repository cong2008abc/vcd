#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "define.h"

class Global {
public:
    static int query_method;
    static const char *db_path;

    static int sub_num;

    static const char *feat_path_prefix;
    static const char *feat_path[3];

    static const char *query_path;
    static const char *lib_path;

    static const char *info_db_query;
    static const char *info_db_src;

    static int feature_db_size;
};

#endif
