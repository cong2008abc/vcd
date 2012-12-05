#ifndef _OM_HPP_
#define _OM_HPP_

//#include <cv.h>
typedef unsigned char uint8;

int check_image_entropy(uint8 *data, int w, int h);
void get_real_feature(const uint8 *data, int w, int h, int N2,
        uint8 *orderAvg, uint8 *orderEnt);

#endif
