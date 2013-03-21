/*
 * the file is try simulate the the data stream in the online
 * system.
 *
 * open a dir and read all images in it.
 * return the stream of the images.
 *
 */
#ifndef _IMITATION_H_
#define _IMITATION_H_

#include "define.h"
#include "video_processor.h"

namespace cv {
    class Mat;
}

namespace vcd {
class Dir;

class Imitation {
public:
    Imitation();
    ~Imitation();

    bool OpenJpgDb(const char *path);
    bool GetNextImg(uint8 *data, int buff_size, int *w, int *h);
    bool GetVideo(process_func f);

    static bool ProcessVideo(const char *path, process_func f);
    static bool ReadImg(const char *path, uint8* data, int buff_size,
                        int *w, int *h);
    static bool CvtYUV2Mat(const uint8 *data, int w, int h, cv::Mat*);
    static bool CvtMat2YUV(const cv::Mat&, uint8 *data, int *w, int *h);

private:
    Dir *_img_db;
};

} //namespace vcd

#endif
