#ifndef _OM_INTERFACE_H_
#define _OM_INTERFACE_H_

extern "C" {
    int open_db(char *);
    int close_db();
    int query_image(unsigned char *data, int w, int h);
};

#endif
