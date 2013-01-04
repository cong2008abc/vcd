#include "frame.h"
#include <sstream>
#include <iostream>

namespace vcd {

const float HIST_THRESHOLD = 0.2;

Frame::Frame() {
    _om_dt = new ImpOMFeature();
    _color_dt = new ColorHistFeature();
}

Frame::~Frame() {
    delete _om_dt;
    delete _color_dt;
}

bool Frame::ExtractFeature(const uint8 *data, int w, int h) {
    _om_dt->ExtractFrame(data, w, h);
    _color_dt->ExtractFrame(data, w, h);

    // generate hash key based on the om feature.
    ImpOMFeature *om_feat = dynamic_cast<ImpOMFeature*>(_om_dt);
    _hash_key = 0;
    int half = ImpOMFeature::FEATURE_LEN / 2;
    for (int i = 0; i < ImpOMFeature::FEATURE_LEN; ++i) {
        _hash_key = _hash_key << 1;
        if (om_feat->_arr_color[i] >= half) {
            _hash_key = _hash_key | 0x1;
        }
    }

    // generate string hash key based on the om feature
    std::stringstream ss;
    for (int i = 0; i < ImpOMFeature::FEATURE_LEN; ++i) {
        if (om_feat->_arr_color[i] < 10) {
            ss << 0 << (int)(om_feat->_arr_color[i]);
        } else {
            ss << (int)(om_feat->_arr_color[i]);
        }
    }
    //ss << "\n";
    ss >> _str_hash_key;
    _str_hash_key += "\n";

    //std::cout << _str_hash_key << std::endl;
}

bool Frame::EqualOM(const Frame *ptr) {
    const ImpOMFeature *left = dynamic_cast<const ImpOMFeature*>(_om_dt);
    const ImpOMFeature *right = dynamic_cast<const ImpOMFeature*>(ptr->_om_dt);

    for (int i = 0; i < ImpOMFeature::FEATURE_LEN; ++i) {
        if (left->_arr_color[i] != right->_arr_color[i]) {
            return false;
        }
    }
    return true;
}

bool Frame::EqualHist(const Frame *ptr) {
    Feature *left = _color_dt;
    const Feature *right = ptr->_color_dt;

    if (left->Compare(right) < HIST_THRESHOLD) {
        return true;
    }
    return false;
}

uint64 Frame::GetHashKey() const{
    return _hash_key;
}

const std::string& Frame::GetStrKey() const {
    return _str_hash_key;
}

bool Frame::SetKey(uint32 key) {
    _key = key;
}

uint32 Frame::GetKey() {
    return _key;
}

} // namespace vcd
