#ifndef _VCD_DIR_H_
#define _VCD_DIR_H_

#include <string>
#include <vector>

struct dirent;

namespace vcd {

/*
 * my simple directory class
 * to read the files in the dir
 *
 */
class Dir {
public:
    Dir();
    explicit Dir(const char *path);
    ~Dir();

    /*
     * every operator need open the dir first.
     * 
     * @path    the dir path
     *
     */
    bool OpenDir(const char *path);

    /*
     * read the next file in the dir
     *
     * @file_name   the next file_name return
     *
     */
    bool GetNextFile(std::string *file_name);

    /*
     * read all files in the dir
     */
    bool GetAllFile(std::vector<std::string> *file_list);

    /*
     * if the GetNextFile come to the last file
     * use reset to the start file
     */
    bool Reset();

    /*
     * close the dir to release the space allocated
     */
    bool CloseDir();


    /*
     * Open a file, if not exists create it.
     */
    FILE *OpenFile(const char *path, const char *flag = "w");
    bool DeleteFile(const char *path);

    // return the num of the files in the dir if dir open
    // else return -1;
    int GetFileNum();

private:
    // dir path
    std::string _path;
    // c struct to store all the file names
    struct dirent **_namelist;
    // flag to whether open a dir
    bool _is_open;
    // files num
    int _file_num;
    // idx to file reading
    int _idx;
};

} // namespace vcd

#endif
