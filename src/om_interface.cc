#include "om_interface.h"
#include "frame.h"
#include "frame_index.h"
#include <string>
#include <stdio.h>
#include <string.h>

vcd::FrameIndex *feature_idx;
char path[128];

const char *YUV = "/yuv/";
const char *DB = "/db/";

void save_img(const vcd::Frame *frame, int idx, unsigned char *data, int w, int h) {
    const std::string &key = frame->GetStrKey();
    char jpg_name[256];

    sprintf(jpg_name, "%s%s%s_%d.yuv", path, YUV, key.c_str(), idx);
    fprintf(stderr, "%s\n", jpg_name);
    FILE *pf = fopen(jpg_name, "w");
    if (pf == NULL) {
        fprintf(stderr, "Save Img Error!\n");
        return;
    }

    // first write weight and height
    fwrite(&w, sizeof(int), 1, pf);
    fwrite(&h, sizeof(int), 1, pf);

    // then write the data of Img
    fwrite(data, sizeof(unsigned char), w * h, pf);

    fclose(pf);
}

int open_db(char *db_path) {
    feature_idx = new vcd::FrameIndexS();
    strcpy(path, db_path);
}

int close_db() {
    delete feature_idx; 
}

int query_image(unsigned char *data, int w, int h) {
    vcd::Frame *frame = new vcd::Frame();
    frame->ExtractFeature(data, w, h);

    int idx;
    int result = feature_idx->AddFrame(frame, &idx);
    if (result == vcd::NO_ONE) {
        save_img(frame, idx, data, w, h);
    } else if (result == vcd::CONFICT) {
        save_img(frame, idx, data, w, h);
    } else {
        // be cause this feature was not insert into the idx,
        // so delete it.
        fprintf(stderr, "Same feature\n");
        delete frame;
    }

    /*
     * the frame has insert into the index structure, so it 
     * was controled by the idx, it don't need to do anything here.
     */
}


