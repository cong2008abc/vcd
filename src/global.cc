#include "global.h"

int Global::query_method = FEAT_SA_OM;
const char* Global::db_path = "../temp";

int Global::sub_num = 8;

// control the path to build the feature
const char* Global::feat_path_prefix = "../feature_om_cut";
const char* Global::feat_path[3] = {"om", "imp_om", "sa_om"};

// control the path to read the feature
const char* Global::query_path = "../feature_om_cut/sa_om/";
const char* Global::lib_path = "../feature_om3/sa_om/";

// control the path of info db
const char* Global::info_db_query = "../info/attack_cur_more.db";
const char* Global::info_db_src = "../info/src.db";

// control the bin of hash in feature_db
int Global::feature_db_size = 1;
