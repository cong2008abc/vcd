#include "imitation.h"
#include "utils.h"
#include "base/vcd_dir.h"
#include <cv.h>

namespace vcd {

Imitation::Imitation() {
    _img_db = new Dir(); 
}

Imitation::~Imitation() {
    delete _img_db;
}

bool Imitation::OpenJpgDb(const char *path) {
    return _img_db->OpenDir(path);
}

bool Imitation::GetNextImg(uint8 *data, int buff_size, int *w, int *h) {
    std::string file;
    
    while (1) {
        // if no file next, return false
        if (_img_db->GetNextFile(&file) == false) {
            return false;
        }

        // if this file is not a pic, try next file
        fprintf(stderr, "%s\n", file.c_str());
        if (Imitation::ReadImg(file.c_str(), data, buff_size, w, h)) {
            break;
        }
    }
    return true;
}

bool Imitation::ReadImg(const char *path, uint8 *data, int buff_size,
                        int *w, int *h) {
    return simulate_input(path, data, buff_size, w, h);
}

} // namespace vcd
