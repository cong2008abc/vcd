#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <stdio.h>
#include <cv.h>

namespace vcd {

typedef unsigned char uint8;
typedef unsigned long long uint64;

enum FEATURE_TYPE {
    kBaseOM = 0x1,
    kImprovedOM = 0x2
};

class Feature {
    friend class Frame;
public:
    Feature() {};
    virtual ~Feature() {};

    virtual bool ExtractFrame(const uint8 *data, int, int) = 0;
    virtual bool DumpToFile(FILE *pfile) = 0;
    virtual bool ReadFromFile(FILE *pfile) = 0;
    
    virtual float Compare(const Feature *rf) = 0;
    //virtual uint32
    
    int GetKeyId();
    int SetKeyId(int key_id);

protected:
    //
private:
    unsigned int _feature_id;
};

class ImpOMFeature: public Feature {
    friend class Frame;
    static int FEATURE_LEN;
public:
    ImpOMFeature();
    ~ImpOMFeature();

    virtual bool ExtractFrame(const uint8 *, int, int);
    virtual bool ExtractFrame(const uint8 *data, int, int,
                              int n = ImpOMFeature::FEATURE_LEN);
    virtual bool DumpToFile(FILE *pfile);
    virtual bool ReadFromFile(FILE *pfile);

    virtual float Compare(const Feature *rf);

    bool GetCompressFeature(uint8 *data) const;

private:
    float InterCompare(const uint8 *arr_a, const uint8 *arr_b);
    bool ExtractIndex(const uint8 *data, int *idx_a, int *idx_b);
    bool CompressFeature(const uint8 *data, uint8 *result, int n) const;

private:
    unsigned char *_arr_color;
    unsigned char *_arr_entropy;

    /*
     * for index, extract from the orignal arr of color and entropy
     */
    uint64 idx_a;
    uint64 idx_b;
    uint64 idx_c;
    uint64 idx_d;
};

Feature *FeatureFactory(int);

}; // namespace vcd

#endif 
