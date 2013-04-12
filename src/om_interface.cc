#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <pthread.h>


//#ifdef _TEST_OM
//#include "om_interface.h"
//#include "frame.h"
//#include "thread_safe_index.h"
//#include "vcd_file.h"
//#include "image_buf.h"
//
//vcd::IndexLRU *frame_index;
//vcd::File *omf_file;
//char path[128];
//vcd::ImageBuf *image_buffer;
//
//const char *YUV = "/yuv/";
//const char *DB = "/db/";
//
//int open_db(char *db_path) {
//    frame_index = new vcd::IndexLRU(1024u * 500);
//    strcpy(path, db_path);
//
//    omf_file = new vcd::File(db_path, "omf");
//    image_buffer = vcd::image_buffer_init(100, 3, db_path);
//
//    printf("****************************************\n");
//    printf("*\n");
//    printf("* om feature is runing!\n");
//    printf("*\n");
//    printf("****************************************\n");
//
//    return 0;
//}
//
//int close_db() {
//    delete frame_index; 
//    delete omf_file;
//
//	return 0;
//}
//
//int query_image(unsigned char *data, int w, int h) {
//    vcd::Frame *frame = new vcd::Frame();
//    frame->ExtractFeature(data, w, h);
//
//    int status = frame_index->InsertThreadSafe(frame);
//    vcd::uint64 feature = frame->GetComprsFeature();
//    switch(status) {
//        case vcd::REPEAT:
//            vcd::insert_repeat_feature(feature);
//        case vcd::NEW:
//            omf_file->Append(&feature, sizeof(feature));
//            image_buffer->AppendImage(data, w, h, feature); 
//            break;
//        case vcd::EXIST:
//            delete frame;
//            break;
//        default:
//            break;
//    }
//
//    return 0;
//}
//#else
//

#include "om_interface.h"
#include "feature_db.h"
#include "info_db.h"
#include "utils.h"
#include "feature/feature.h"
#include <list>
#include <vector>

vcd::FeatureDB *db;
vcd::InfoDB *info_db;

int open_db(char *path) {
    db = new vcd::FeatureDB();
    info_db = new vcd::InfoDB();

    db->OpenDB(path);
    info_db->OpenDB("../info/info.db");

    return 0;
}

int close_db() {
    return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::ImpOMFeature imp;

    imp.ExtractFrame(data, w, h, 8);
    std::list<const vcd::Feature*> r;
    r = db->Query(&imp);

    //show_yuv_colorful(data, w, h);
    std::vector<std::string> result; 
    for(std::list<const vcd::Feature*>::iterator it = r.begin();
        it != r.end(); ++it) {
        const vcd::Feature *ptr = *it;
        std::string path(info_db->GetItem(ptr->GetKeyId()));
        result.push_back(path.substr(0, path.find(".jpg") + 4));
        printf("%d %s\n",(*it)->GetKeyId(),
                            info_db->GetItem((*it)->GetKeyId()),
                            imp.Compare(ptr));
    }
    show_query_result(data, w, h, result);

    return 0;
}
//#endif
