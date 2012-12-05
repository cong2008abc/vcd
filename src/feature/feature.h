#ifndef _FEATURE_H_
#define _FEATURE_H_

#include <stdio.h>

namespace vcd {

typedef unsigned char uint8;

enum FEATURE_TYPE {
    kBaseOM = 0x1,
    kImprovedOM = 0x2
};

class Feature {
public:
    Feature() {};
    virtual ~Feature() {};

    virtual bool ExtractFrame(const uint8 *data, int, int) = 0;
    virtual bool DumpToFile(FILE *pfile) = 0;
    virtual bool ReadFromFile(FILE *pfile) = 0;
    
    virtual float Compare(const Feature &rf) = 0;
    
    int GetKeyId();
    int SetKeyId(int key_id);

private:
    unsigned int _feature_id;
};

class ImpOMFeature: public Feature {
public:
    ImpOMFeature();
    ~ImpOMFeature();

    virtual bool ExtractFrame(const uint8 *data, int, int);
    virtual bool DumpToFile(FILE *pfile);
    virtual bool ReadFromFile(FILE *pfile);

    virtual float Compare(const Feature &rf);

    static int FEATURE_LEN;

private:
    unsigned char *_arr_color;
    unsigned char *_arr_entropy;
};

Feature *FeatureFactory(int);

}; // namespace vcd

#endif 
