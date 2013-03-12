#include "frame.h"
#include <sstream>
#include <iostream>

namespace vcd {

const float HIST_THRESHOLD = 0.2;

Frame::Frame(): _hash_key(0), _str_hash_key("Null"){
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
    ImpOMFeature *om_feat = _om_dt;
//    _hash_key = 0;
//    int half = ImpOMFeature::FEATURE_LEN / 2;
//    for (int i = 0; i < ImpOMFeature::FEATURE_LEN; ++i) {
//        _hash_key = _hash_key << 1;
//        if (om_feat->_arr_color[i] >= half) {
//            _hash_key = _hash_key | 0x1;
//        }
//    }

    if (ImpOMFeature::LEN == 16) {
        uint8 *p = reinterpret_cast<uint8*>(&_om_16i);
        om_feat->GetCompressFeature(p);
    }

    return true;

//    std::stringstream ss;
//    for (int i = 0; i < ImpOMFeature::FEATURE_LEN; ++i) {
//        if (om_feat->_arr_color[i] < 10) {
//            ss << 0 << (int)(om_feat->_arr_color[i]);
//        } else {
//            ss << (int)(om_feat->_arr_color[i]);
//        }
//    }
//    //ss << "\n";
//    ss >> _str_hash_key;
//    _str_hash_key += "\n";

    return true;
}

bool Frame::EqualOM(const Frame *ptr) {
    /*
     * for 4*4 om, use the compress result to compare
     */
    if (_om_16i != 0) {
        if (_om_16i != ptr->_om_16i) {
            return false;
        } else {
            return true;
        }
    }

    const ImpOMFeature *left = _om_dt;
    const ImpOMFeature *right = ptr->_om_dt;

    for (int i = 0; i < ImpOMFeature::LEN; ++i) {
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

bool Frame::DumpToFile(FILE *pf) {
	const ColorHistFeature *ptr_color = dynamic_cast<const ColorHistFeature*>(_color_dt);
	fwrite(ptr_color->_arr_hist, sizeof(float), ColorHistFeature::FEATURE_LEN, pf);	
    return true;
}

uint64 Frame::GetHashKey() const{
    return _hash_key;
}

const std::string& Frame::GetOMStr() const {
    return _str_hash_key;
}

bool Frame::SetKey(uint32 key) {
    _key = key;
    return true;
}

uint32 Frame::GetKey() {
    return _key;
}

const uint64 Frame::GetComprsFeature() const {
    return _om_16i;
}

} // namespace vcd
