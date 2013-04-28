#include "om_feature.h"
#include "om.h"

namespace vcd {

//
// base class [ OM ]
//
OM::OM() {
}

OM::~OM() {
}

bool OM::SetID(uint64 id) {
    _key_id = id;
    return true;
}

uint64 OM::GetID() {
    return _key_id;
}

bool DumpToFile(FILE *pf) {
    fwrite(&_key_id, sizeof(uint64), 1, pf);
    return true;
}

float OM::OMCompare(uint8 *a, uint8 *b, int len) {
    
}

int OM::BinaryCompare(uint64 a, uint64 b) {
}

uint64 OM::ExtractBinaryIndex(uint8 *a, int len) {
}

int OM::Compress(const uint8 *f, int len, uint8 *ret) {
}

//
// class of Simply OM
//
SimplyOM::SimplyOM(int n): _n(n) {
    _arr_color = new uint8[n];
}

SimplyOM::~SimplyOM() {
    if (_arr_color != NULL) {
        delete [] _arr_color;
    }
}

float SimplyOM::Compare(const OM *rf_) const {
    const SimplyOM *rf = dynamic_cast<const SimplyOM*>(rf_); 
    return OMCompare(this->_arr_color, rf->_arr_color, _n);
}

float SimplyOM::SpeedCompare(const OM *_rf, int diff) const {
    const SimplyOM *rf = dynamic_cast<const SimplyOM*>(rf_);

    if(BinaryCompare(this->_binary_idx, rf->_binary_idx) > diff) {
        return -1.0;
    }

    return OMCompare(_arr_color, rf->_arr_color, _n);
}

bool SimplyOM::DumpToFile(FILE *pf) {
    OM::DumpToFile(pf);
    fwrite(&_n, sizeof(int), 1, pf);
    fwrite(_arr_color, sizeof(uint8), _n, pf);
    fwrite(&_binary_idx, sizeof(uint64), 1, pf);

    return true;
}

SimplyOM* SimplyOM::Extract(const uint8 *data, int w, int h, int n) {
    SimplyOM *feature = new SimplyOM(n);
    get_real_feature(data, w, h, n, feature->_arr_color, NULL);

    return feature;
}

SimplyOM* SimplyOM::ReadFromFile(FILE *pf) {
    //
}

} // namespace vcd
