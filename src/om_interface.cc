#include "om_interface.h"
#include "frame.h"
#include "thread_safe_index.h"
#include "vcd_file.h"
#include "image_buf.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <pthread.h>

vcd::IndexLRU *frame_index;
vcd::VcdFile *omf_file;
char path[128];
vcd::ImageBuf *image_buffer;

const char *YUV = "/yuv/";
const char *DB = "/db/";

int open_db(char *db_path) {
//    char log[128];
//    strcpy(log, db_path);
//    strcat(log, "om.log");

    frame_index = new vcd::IndexLRU(1024u * 500);
    strcpy(path, db_path);

    //new method
    omf_file = new vcd::VcdFile(db_path, "omf");

    image_buffer = vcd::image_buffer_init();

    return 0;
}

int close_db() {
    delete frame_index; 
    delete omf_file;

	return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::Frame *frame = new vcd::Frame();
    frame->ExtractFeature(data, w, h);

    int status = frame_index->InsertThreadSafe(frame);
    switch(status) {
        case vcd::REPEAT:
            vcd::insert_repeat_feature(frame->GetComprsFeature());
        case vcd::NEW:
            omf_file->Append(frame->GetOMStr());
            image_buffer->AppendImage(data, w, h,
                                      frame->GetComprsFeature());
            break;
        case vcd::EXIST:
            // do nothing
            break;
        default:
            break;
    }

//    if (frame_index->InsertThreadSafe(frame) == 0) {
//        omf_file->Append(frame->GetOMStr());
//    } else {
//        delete frame;
//    }
    
    return 0;
}


