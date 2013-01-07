#include "om_interface.h"
#include "frame.h"
#include "frame_index.h"
#include "vcd_file.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <map>
#include <pthread.h>

vcd::FrameIndex *feature_idx;
vcd::FrameIndexLRU *feature_lru_idx;
vcd::VcdFile *v_file;
char path[128];
int global_jpg_idx;
std::map<const std::string, int> *str2idx;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

const char *YUV = "/yuv/";
const char *DB = "/db/";

int find_img_idx(const std::string &key) {
    if (str2idx->find(key) == str2idx->end()) {
        str2idx->insert(std::pair<const std::string, int>(key, global_jpg_idx));
        int ret = global_jpg_idx++;
        return ret;
    } else {
        return (*str2idx)[key];
    }
}

void save_img(const vcd::Frame *frame, int idx, unsigned char *data, int w, int h) {
    const std::string &key = frame->GetStrKey();
    char jpg_name[256];
    
    /*
     * get the name of the yuv
     */
    /* named by the idx */
    //int img_idx = find_img_idx(key);
    //sprintf(jpg_name, "%s%s%d_%d.yuv", path, YUV, img_idx, idx);
    /* named by the feature */
    sprintf(jpg_name, "%s%s%s_%d.yuv", path, YUV, key.c_str(), idx);
    //fprintf(stderr, "%s\n", jpg_name);

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
    //feature_idx = new vcd::FrameIndexS();
    feature_lru_idx = new vcd::FrameIndexLRU(1024 * 10);
    strcpy(path, db_path);

    str2idx = new std::map<const std::string, int>();
    global_jpg_idx = 0;

    //new method
    v_file = new vcd::VcdFile(db_path);
		//lock = PTHREAD_MUTEX_INITIALIZER;
}

int close_db() {
    delete feature_idx; 

    delete v_file;

	return 0;
}

int query_image(unsigned char *data, int w, int h) {
    vcd::Frame *frame = new vcd::Frame();
    frame->ExtractFeature(data, w, h);

		pthread_mutex_lock(&lock);
		v_file->AppendFrame(frame);
		pthread_mutex_unlock(&lock);
		delete frame;
//    if (feature_lru_idx->Insert(frame) == 0) {
//        //printf("ok\n");
//        v_file->AppendFrame(frame);
//    } else {
//        delete frame;
//    }

    /*
    int idx;
    int result = feature_idx->AddFrame(frame, &idx);
    if (result == vcd::NO_ONE) {
        save_img(frame, idx, data, w, h);
    } else if (result == vcd::CONFICT) {
        save_img(frame, idx, data, w, h);
    } else {
        // be cause this feature was not insert into the idx,
        // so delete it.
        //fprintf(stderr, "Same feature\n");
        delete frame;
    }*/

    /*
     * the frame has insert into the index structure, so it 
     * was controled by the idx, it don't need to do anything here.
     */
		return 0;
}


