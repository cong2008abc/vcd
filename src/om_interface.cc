#include "om_interface.h"
#include "frame.h"
#include "frame_index.h"
#include <string.h>

vcd::FrameIndex *feature_idx;
char path[128];

const char *YUV = "/yuv/";
const char *DB = "/db/";

int open_db(char *path) {
    feature_idx = new vcd::FrameIndex();
    strcpy(path, path);
}

int close() {
    delete feature_idx; 
}

int query_image(unsigned char *data, int w, int h) {
    vcd::Frame *frame = new vcd::Frame();
    frame->ExtractFeature(data, w, h);

}


