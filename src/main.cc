#include <stdio.h>
const char *test_db_info = "./info.db";

int main() {
    FILE *pf = fopen("/mnt/db/1/pvf-2440318516600075350-320x240.yuv-19.jpg", "r");
    if (pf == NULL) {
        printf("not ok\n");
    }
//    vcd::InfoDB db;
//    
//    db.OpenDB(test_db_info);
//    db.print();
//    db.Insert("1.jpg");
//    db.Insert("2.jpg");
//    db.Insert("3.jpg");
//    db.Dump(test_db_info);
}
