#include "define.h"

namespace vcd {

class OM {
public:
    OM();
    ~OM();

    virtual float Compare(const OM *rf) const = 0;
    virtual float SpeedCompare(const OM *rf, int diff) const = 0;
    //virtual bool Extract(const uint8 *data, int w, int h, int n) = 0;
    bool SetID(uint64 id);
    uint64 GetID();
    
protected:
    bool DumpToFile(FILE *pf);
    float OMCompare(uint8 *a, uint8 *b, int len);
    int BinaryCompare(uint64 a, uint64 b);
    uint64 ExtractBinaryIndex(uint8 *a, int len);
    int Compress(const uint8 *f, int len, uint8 *ret);

private:
    uint64 _key_id;
};


class SimplyOM: public OM {
public:
    SimplyOM(int n);
    ~SimplyOM();

    float Compare(const OM *rf) const;
    float SpeedCompare(const OM *rf, int diff) const;
    static SimplyOM* Extract(const uint8 *data, int w, int h, int n);
    static SimplyOM* ReadFromFile(FILE *pf); 
    bool DumpToFile(FILE *pf);

private:
    uint8 *_arr_color;
    uint64 _binary_idx;
    int _n;
};

class ImprovedOM: public OM {
public:
    ImprovedOM(int n);
    ~ImprovedOM();

    float Compare(const OM *rf) const;
    float SpeedCompare(const OM *rf, int diff) const;
    float SpeedCompare(const OM *rf, int diff, float thres) const;
    static ImprovedOM* Extract(const uint8 *data, int w, int h, int n);
    static ImprovedOM* ExtractWithSaliency(const uint8 *data, int w, int h, int n);

    static ImprovedOM* ReadFromFile(FILE *pf); 
    bool DumpToFile(FILE *pf);

private:
    uint8 *_arr_color;
    uint8 *_arr_entropy;
    uint64 _binary_idx_color;
    uint64 _binary_idx_ent;
    int _n;
};

} // namespace vcd
