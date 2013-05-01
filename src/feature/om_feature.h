/*
 * author: zhouchao(zhouchao@ict.ac.cn
 *
 * this file describe three different features based on OM(ordinal Measure) 
 *
 * [1] class OM is the based class of all features, define some common functions
 *
 * [2] class SimplyOM is the simply use of OM feature, just the sorted array of color
 *
 * [3] class ImprovedOM is the improved version of OM feature, add the 
 * sorted array of entropy and 
 *      add a new function about extracting feature with saliency-area
 *
 */

#include "define.h"

namespace vcd {

class OM {
public:
    OM();
    ~OM();

    virtual float Compare(const OM *rf) const = 0;
    virtual float SpeedCompare(const OM *rf, int diff) const = 0;
    bool SetID(uint64 id);
    uint64 GetID();
    
protected:
    /*
     * common functions for all OM features.
     */

    //
    // output all common data of feature to file
    //
    bool DumpToFile(FILE *pf);

    // 
    // compare two om sorted array with different len
    //
    // @a       is the a sorted array
    // @b       is the b sorted array
    // @len     is the len of sorted array
    //
    // return the difference of two sorted array.
    //
    float OMCompare(const uint8 *a, const uint8 *b, const int len) const;

    //
    // compare the hamming length of two 64 bits num
    //
    int HammCompare(const uint64 a, const uint64 b) const;

    //
    // extract a index bits from the given sorted array
    //
    // @a       is the sorted array
    // @len     is the len of @a
    //
    // @ret     is the result return
    //
    bool ExtractBinaryIndex(const uint8 *a, uint64 *ret, int len);

    //
    // compress the sorted array for saving
    //
    int Compress(const uint8 *f, int len, uint8 *ret);

private:
    uint64 _key_id;
};


class SimplyOM: public OM {
public:
    ~SimplyOM();

    float Compare(const OM *rf) const;
    float SpeedCompare(const OM *rf, int diff) const;
    bool DumpToFile(FILE *pf);

    static SimplyOM* Extract(const uint8 *data, int w, int h, int n);
    static SimplyOM* ReadFromFile(FILE *pf); 

private:
    //
    // simplyOM only can be allocated from the static Extract Function.
    // or the Read function
    //
    SimplyOM(int n);

private:
    //
    // simply om feature only has a sorted array of color
    //
    uint8 *_arr_color;
    //
    // @_binary_idx is the index bit get from the sorted array of color
    //
    uint64 _binary_idx;
    //
    // @_n is the len of sorted array we extracted
    //
    int _n;
};

class ImprovedOM: public OM {
public:
    ~ImprovedOM();

    float Compare(const OM *rf) const;
    float SpeedCompare(const OM *rf, int diff) const;
    float SpeedCompare(const OM *rf, int diff, float thres) const;
    bool DumpToFile(FILE *pf);

    static ImprovedOM* Extract(const uint8 *data, int w, int h, int n);
    static ImprovedOM* ExtractWithSaliency(const uint8 *data, int w, int h, int n);
    static ImprovedOM* ReadFromFile(FILE *pf); 

private:
    //
    // same with SimplyOM, ImprovedOM also only can be allocated from the
    // static function of Extract and ExtractWithSaliency.
    // or the Read Function
    //
    ImprovedOM(int n);

private:
    //
    // same with the SimplyOM feature, but here we get two sorted array,
    // One is for color and another is for entropy.
    //
    uint8 *_arr_color;
    uint8 *_arr_entropy;
    uint64 _binary_idx_color;
    uint64 _binary_idx_ent;
    int _n;
};

} // namespace vcd
