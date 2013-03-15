#include "image_buf.h"
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <pthread.h>
#include <set>

namespace vcd {


struct image_tmp_info {
    int tmp_file_num;           // the tmp file num
    int last_handle_idx;        // last handle tmp file idx
    int last_create_idx;        // last created tmp file idx
    int max_tmp_size;           // use the kbufsize as the unit
    int cur_buf_num;
    FILE *pf_cur;
    char path[64];
    
    // for buf save
    pthread_cond_t buf_cond;
    pthread_mutex_t buf_mutex;

    // for img_tmp handle
    pthread_cond_t io_cond;
    pthread_mutex_t io_mutex;
    bool io_enable;

    // repeat om feature
    std::set<uint64> *feat_repeater;
};
static image_tmp_info *img_info;
static int max_tmp_file_num;
bool exit;
static ImageBuf *image_buffer;

const int kbufsize = 1024 * 1024 * 10;      // for test, 10MB buf

#define CLEAN_BUF(buf)                  \
do {                                    \
    *(reinterpret_cast<int*>(buf)) = 0; \
    _real_buf = buf + sizeof(int);      \
    _tail = _real_buf;                  \
    _end = buf + kbufsize;              \
} while(0)

#define ADD_BUF_NUM(buf)                \
do {                                    \
    *(reinterpret_cast<int*>(buf)) = 0; \
} while(0)

#define SWITCH_UINT8_PTR(a, b)          \
do {                                    \
    uint8 *t = a;                       \
    a = b;                              \
    b = t;                              \
} while(0)

#define COPY_UINT8_DATA(dst_, src_, n)  \
do {                                    \
    uint8 *dst = dst_;                  \
    const uint8 *src = src_;            \
    for (int i = 0; i < n; ++i)         \
        *dst++ = *src++;                \
} while(0)

#define YUV_IMAGE_SIZE(w, h) ((w * h * 3 / 2))

ImageBuf::ImageBuf() {
    _buf[0] = new uint8[kbufsize];
    _buf[1] = new uint8[kbufsize];
    CLEAN_BUF(_buf[0]);
}

ImageBuf::~ImageBuf() {
}

bool ImageBuf::AppendImage(const uint8 *data, int w, int h,
                           uint64 feat) {
    MutexLockGuard lg(_locker);
    // calc the size of the image,
    // include sizeof info struct and image content,
    // in YYUV format
    int image_size = sizeof(image_node) + YUV_IMAGE_SIZE(w, h);
    if (_tail + image_size >= _end) {
        // switch the buf, and wake buf saver!
        SWITCH_UINT8_PTR(_buf[0], _buf[1]);
        CLEAN_BUF(_buf[0]);

        pthread_cond_signal(&img_info->buf_cond);
    } 

    image_node *node = reinterpret_cast<image_node*>(_tail);
    node->om_16i = feat;
    node->w = w;
    node->h = h;
    COPY_UINT8_DATA(node->data, data, YUV_IMAGE_SIZE(w, h));
    _tail += image_size;
    ADD_BUF_NUM(_buf[0]);
    return true;
}

const uint8* ImageBuf::GetData() {
    return _buf[1]; 
}

void *thread_save_buf(void *arg) {
    const uint8 *ptr = reinterpret_cast<const uint8*>(arg);
    while (!exit) {
        // here is a simple cond use.
        // i think the io write operator is more faster than
        // the network processers full the buf,
        // and because we only have 2 buf, if the network is
        // faster than io writer, the data will lose.
        //
        // so if the io writer is more faster, this thread will
        // always wait for the signal sent by network processers
        pthread_cond_wait(&img_info->buf_cond,
                          &img_info->buf_mutex);

        if (img_info->pf_cur == NULL) {
            char name[128];
            sprintf(name, "%s/%d_tmp.imgdb", img_info->path,
                                             img_info->last_create_idx);
            img_info->pf_cur = fopen(name, "w");
            if (img_info->pf_cur == NULL) {
                fprintf(stderr, "someting terrible happens\n");
                continue;
            }
            img_info->cur_buf_num = 0;
        }
        fwrite(ptr, sizeof(uint8), kbufsize, img_info->pf_cur);
        img_info->cur_buf_num++;

        if (img_info->cur_buf_num == img_info->max_tmp_size) {
            fclose(img_info->pf_cur);
            img_info->pf_cur = NULL;
            img_info->last_create_idx++;
            
            pthread_mutex_lock(&img_info->io_mutex);
            img_info->tmp_file_num++;
            
            // if the tmpfile handle thread is sleep
            // and we need it to handle the tmpfile,
            // wake up it!!!
            if (img_info->tmp_file_num >= max_tmp_file_num &&
                img_info->io_enable == false) {
                img_info->io_enable = true;
                pthread_cond_signal(&img_info->io_cond);
            }
            pthread_mutex_unlock(&img_info->io_mutex);
        }
    }
    return NULL;
}

void *thread_tmp_handler(void *arg) {
    while (!exit) {
        pthread_mutex_lock(&img_info->io_mutex);
        if (img_info->tmp_file_num < max_tmp_file_num) {
            img_info->io_enable = false;
            pthread_cond_wait(&img_info->io_cond, &img_info->io_mutex);
        }
        img_info->tmp_file_num--;
        pthread_mutex_unlock(&img_info->io_mutex);

        char name[128];
        sprintf(name, "%s/%d_tmp.imgdb", img_info->path,
                                         img_info->last_create_idx);
        img_info->pf_cur = fopen(name, "w");
        FILE *pf = fopen(name, "r");
        if (pf == NULL) {
            fprintf(stderr, "something terrible happens!\n");
            continue;
        }

        // hanle the tmp file
        int k = img_info->max_tmp_size;
        while (k--) {
            int buf_num;
            fread(&buf_num, sizeof(int), 1, pf);
            image_node node;
            uint8 *data_buf[320 * 160 * 3];
            for (int i = 0; i < buf_num; ++i) {
                fread(&node, sizeof(image_node), 1, pf);
                if (img_info->feat_repeater->find(node.om_16i) !=
                    img_info->feat_repeater->end()) {
                    // TODO(zc) save this image
                } else {
                    fseek(pf, node.w * node.h * 3 / 2, SEEK_CUR);
                }
            }
        }
    }
    return NULL;
}

ImageBuf* image_buffer_init(int buf_num_of_file, int max_tmp_num,
                            const char *db_path) {
    img_info = new image_tmp_info;

    img_info->tmp_file_num = 0;
    img_info->last_handle_idx = 0;
    img_info->last_create_idx = 0;
    img_info->max_tmp_size = buf_num_of_file;

    pthread_mutex_init(&img_info->io_mutex, NULL);
    pthread_mutex_init(&img_info->buf_mutex, NULL);
    pthread_cond_init(&img_info->io_cond, NULL);
    pthread_cond_init(&img_info->buf_cond, NULL);
    img_info->io_enable = false;

    img_info->pf_cur = NULL;
    strcpy(img_info->path, db_path);
    img_info->feat_repeater = new std::set<uint64>();

    max_tmp_file_num = max_tmp_num;
    exit = false;
    image_buffer = new ImageBuf();

    // create thread
    pthread_t tid[2];
    pthread_create(&tid[0], NULL, thread_save_buf,
                   (void*)(image_buffer->GetData()));
    pthread_create(&tid[1], NULL, thread_tmp_handler, NULL);

    return image_buffer;
}

void insert_repeat_feature(uint64 om_16i) {
    img_info->feat_repeater->insert(om_16i);
}

void close_image_buffer() {
    exit = true;    
}

} // namespace vcd
