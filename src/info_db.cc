#include "info_db.h"
#include <stdio.h>
#include <string.h>

namespace vcd {

InfoDB::InfoDB() {
    _last_idx = 0;
}

InfoDB::~InfoDB() {
    std::map<uint32, char *>::iterator it;
    for (it = _ele_map.begin(); it != _ele_map.end(); ++it) {
        char *name = it->second;
        delete [] name;
    }
}

/*
 * the db store the info like this:
 * [last_idx(uint32)]
 * [key_id(uint32)-path_len(uint32)-path(path_len)]
 * //repeat...
 */
bool InfoDB::OpenDB(const char *db_file) {
    FILE *pf = fopen(db_file, "r");
    if (pf == NULL) {
        return false;
    }

    uint32 ret;
    // first read the last_idx;
    ret = fread(&_last_idx, sizeof(uint32), 1, pf);
    if (ret != 1) {
        fclose(pf);
        return false;
    }
    while (1) {
        // read the key_id
        uint32 key_id;
        ret = fread(&key_id, sizeof(uint32), 1, pf);
        if (ret != 1) {
            break;
        }

        // read the content
        uint32 size;
        ret = fread(&size, sizeof(uint32), 1, pf);
        if (ret != 1 || size <= 0) {
            break;
        }

        char *content = new char[size];
        ret = fread(content, sizeof(char), size, pf);
        if (ret != size) {
            fclose(pf);
            return false;
        }

        _ele_map.insert(std::pair<uint32, char *>(key_id, content));
    }

    fclose(pf);
    return true;
}

uint32 InfoDB::Insert(const char *path) {
    uint32 cur_idx = _last_idx++;
    char *content = new char[strlen(path) + 1];
    strcpy(content, path);

    _ele_map.insert(std::pair<uint32, char *>(cur_idx, content));
    
    return cur_idx;
}

bool InfoDB::Dump(const char *db_path) {
    FILE *pf = fopen(db_path, "w+");
    if (pf == NULL) 
        return false;

    fwrite(&_last_idx, sizeof(uint32), 1, pf);
    std::map<uint32, char *>::iterator it;
    for (it = _ele_map.begin(); it != _ele_map.end(); ++it) {
        char *cur_path = it->second;
        int size = strlen(cur_path);
        fwrite(&(it->first), sizeof(uint32), 1, pf);
        fwrite(&size, sizeof(uint32), 1, pf);
        fwrite(cur_path, sizeof(char), size, pf);
    }
    
    fclose(pf);
    return false;
}

void InfoDB::print() {
    std::map<uint32, char *>::iterator it;
    for (it = _ele_map.begin(); it != _ele_map.end(); ++it) {
        printf("%d - %s\n", it->first, it->second);
    }
    printf("--------------\n");
}

} // namespace vcd
