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
    virtual float Compare(const Feature *rf, float thres);
    //virtual uint32
    
    int GetKeyId() const;
    int SetKeyId(int key_id);

    virtual void print() const = 0;

protected:
    //
private:
    unsigned int _feature_id;
};

class ImpOMFeature: public Feature {
    friend class Frame;
    static int LEN;
public:
    ImpOMFeature();
    ~ImpOMFeature();

    virtual bool ExtractFrame(const uint8 *, int, int);
    virtual bool ExtractFrame(const uint8 *data, int, int, int);
    virtual bool DumpToFile(FILE *pfile);
    virtual bool ReadFromFile(FILE *pfile);

    virtual float Compare(const Feature *rf);
    virtual float Compare(const ImpOMFeature *rf);
    virtual float Compare(const Feature *rf, float thres);

    bool GetCompressFeature(uint8 *data) const;
    virtual void print() const;

protected:
    float InterCompare(const uint8 *arr_a, const uint8 *arr_b);

private:
    bool ExtractIndex(const uint8 *data, uint64 *idx_b);
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

class SimplyOMFeature: public ImpOMFeature {
public:
    SimplyOMFeature();
    ~SimplyOMFeature();

    virtual bool ExtractFrame(const uint8 *data, int w, int h, int n);
    virtual float Compare(SimplyOMFeature *rf, float thres);
};

class SaliencyOMFeature: public ImpOMFeature {
public:
    SaliencyOMFeature();
    ~SaliencyOMFeature();
    virtual bool ExtractFrame(const uint8 *data, int w, int h, int n);
};

Feature *FeatureFactory(int);

}; // namespace vcd

#endif 
