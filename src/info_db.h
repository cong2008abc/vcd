#ifndef _INFO_DB_H_
#define _INFO_DB_H_

#include <map>

namespace vcd {

typedef unsigned int uint32;

class InfoDB {
public:
    InfoDB();
    ~InfoDB();

    bool OpenDB(const char *db_path);
    uint32 Insert(const char *path);
    bool Dump(const char *db_path);

    /*
     * test function
     */
    void print();

private:
    std::map<uint32, char *> _ele_map;
    uint32 _last_idx;
};

} // namespace vcd

#endif
