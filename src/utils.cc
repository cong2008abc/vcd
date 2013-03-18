#include "utils.h"
#include "base/logging.h"
#include "feature/common.h"

#include <stdio.h>
#include <highgui.h>

/*
 * show IplImage!
 */

void show_image(const IplImage *src, const char *name) {
    cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
    cvShowImage(name, src);
    cvWaitKey(0);
}

/*
 * the component of y is the YUV image is also the 
 * gray image, so this function do the same thing as
 * the funtion(get_gray) blow
 */

IplImage *get_component_Y(const IplImage *src) {
    IplImage *yuv = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);    
    cvCvtColor(src, yuv, CV_BGR2YCrCb);
    IplImage *compY = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvSplit(yuv, compY, NULL, NULL, 0);
//  show_image(yuv, "yuv");
//  show_image(compY, "component y");

    cvReleaseImage(&yuv);
    return compY;
}

IplImage *get_gray(const IplImage *src) {
    IplImage *gray = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    cvCvtColor(src, gray, CV_BGR2GRAY);
//  show_image(gray, "gray");

    return gray;
}

float pixval32f(const IplImage *src, int w, int h) {
    return ((float*)(src->imageData + h * src->widthStep))[w];
}

unsigned char get_img_val(const IplImage *src, int w, int h) {
    unsigned char *ptr = (unsigned char *)(src->imageData + h * src->widthStep);
    //printf("%d\n", ptr[0]);
    return ptr[w];
}

void set_img_val(IplImage *src, int w, int h, unsigned char val) {
    unsigned char *ptr = (unsigned char *)(src->imageData + h * src->widthStep);

    ptr[w] = val;
}

void set_pixval32f(const IplImage *src, int w, int h, float val) {
    float *ptr = (float *)(src->imageData + h * src->widthStep);

    ptr[w] = val;
}

bool iplImage2uint8point(IplImage *src, uint8 *data) {
    int w = src->width;
    int h = src->height;

    int idx = 0;
    for (int i = 0; i < h; i++) {
        uint8 *ptr = (uint8 *)(src->imageData + i * src->widthStep);
        for (int j = 0; j < w; j++) {
            //if (ptr[j] != 16) printf("%d %d\n", j, i);
            //printf("%d ", idx);
            data[idx++] = ptr[j];
        }
    }

    return true;
}

void show_image_from_path(const char *path) {
    IplImage *img = cvLoadImage(path);
    show_image(img, "a");

    cvReleaseImage(&img);
}

void show_image_from_path(const char *path, const char *pathB) {
    IplImage *img = cvLoadImage(path);
    show_image(img, "a");
    IplImage *imgB = cvLoadImage(pathB);
    show_image(imgB, "b");
}

bool load_jpg_image(const char *path, int &w, int &h, uint8 *data, int len) {
    IplImage *img = cvLoadImage(path);
    if (img == NULL) {
        printf("Load Jpg error, file not exists! %s\n", path);
        return false;
    }
    IplImage *comY;
    if (img->nChannels != 1) {
        comY = get_component_Y(img);
    } else {
        comY = img;
    }

	w = comY->width;
	h = comY->height;	
    if (w * h > len) {
        fprintf(stderr, "Image is to large!\n");
        return false;
    }

    iplImage2uint8point(comY, data);

    if (img->nChannels != 1) {
        cvReleaseImage(&comY);
    }
    cvReleaseImage(&img);

    /*
     * test imread
     */
//    cv::Mat mt_img = cv::imread(path);
//    VLOG(0, "mat dims: %d", mt_img.dims);
//    //fprintf(stdout, "mat dims: %d\n", mt_img.dims);
//    mt_img.convertTo(mt_img, CV_8U);
//    cv::cvtColor(mt_img, mt_img, CV_BGR2YUV);
//    uint8 *data_ptr = (uint8*)(mt_img.data);
//    int i = 0;
//    while (i++ < mt_img.cols * mt_img.rows * mt_img.dims) {
//        *data++ =*data_ptr++;
//    }

    return true;
}

bool load_image_with_color(const char *path, int &w, int &h, uint8 *data, int len) {
    if (path == NULL) {
        fprintf(stderr, "NULL Pointer. %s %d\n", __FILE__, __LINE__);
        return false;
    }

    IplImage *img = cvLoadImage(path);
    if (img == NULL) {
        fprintf(stderr, "Error Image Path. %s %d\n", __FILE__, __LINE__);
        return false;
    }

    w = img->width;
    h = img->height;
    if (img->nChannels == 1) {
        if (w * h > len) {
            fprintf(stderr, "Image is too large! %s %d\n", __FILE__, __LINE__);
            goto fail; 
        }
        iplImage2uint8point(img, data);
    } else if (img->nChannels == 3) {
        if (w * h * 3 > len)  {
            fprintf(stderr, "Image is too large! %s %d\n", __FILE__, __LINE__);
            goto fail;
        } 
        CvScalar p;
        int idx = 0;
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                p = cvGet2D(img, i, j);
                data[idx++] = p.val[0];
                data[idx++] = p.val[1];
                data[idx++] = p.val[2];
            }
        }
    } else {
        goto fail;
    }

//succ:
    cvReleaseImage(&img);
    return true;

fail:
    cvReleaseImage(&img);
    return false;
}

IplImage *yuv2iplImage(const uint8* data, int w, int h) {
    IplImage *img = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 1);
    int idx = 0;
    for (int i = 0; i < h; i++) {
        uint8* ptr = (uint8*)(img->imageData + i * img->widthStep);
        for (int j = 0; j < w; j++)
            ptr[j] = data[idx++];
    }

    return img;
}

void show_yuv(const uint8* data, int w, int h) {
    IplImage *img = yuv2iplImage(data, w, h);
    show_image(img, "b");

    cvReleaseImage(&img);
}

void show_yuv_and_path(const uint8* data, int w, int h, const char *path) {
    IplImage *query = yuv2iplImage(data, w, h);
    IplImage *matchOri = cvLoadImage(path);
    if (matchOri == NULL) {
        show_image(query, "no match");
    } else {
        int sh = query->height > matchOri->height ? query->height : matchOri->height;
        int sw = query->width + matchOri->width + 10;

        IplImage *matchY = get_component_Y(matchOri);
        IplImage *showImg = cvCreateImage(cvSize(sw, sh), IPL_DEPTH_8U, 1);
        
        cvZero(showImg);
        cvSetImageROI(showImg, cvRect(0, 0, query->width, query->height));
        cvAdd(showImg, query, showImg, NULL);
        //show_image(showImg, "match");
        cvSetImageROI(showImg, cvRect(query->width + 10, 0, matchOri->width, matchOri->height));
        cvAdd(showImg, matchY, showImg, NULL);
        //show_image(showImg, "match");
        cvResetImageROI(showImg);

        //show_image(matchY, "yuv");
        show_image(showImg, "match");

        cvReleaseImage(&showImg);
        cvReleaseImage(&matchY);
        cvReleaseImage(&matchOri);
    }
    cvReleaseImage(&query);
}

void drawPoint(IplImage *src, int w, int h) {
//    cvCircle(src, cvPoint(w, h), 10, cvScalar(0, 255, 0), 1);
    cvLine(src, cvPoint(w - 5, h), cvPoint(w + 5, h), cvScalar(0, 0, 255), 1);
    cvLine(src, cvPoint(w, h - 5), cvPoint(w, h + 5), cvScalar(0, 0, 255), 1);
}

void drawPoint( IplImage *src, int w, int h, int color ) {
    cvLine( src, cvPoint( w - 5, h ), cvPoint( w + 5, h ), cvScalar( 0, 0, color ), 1 );
    cvLine( src, cvPoint( w, h - 5 ), cvPoint( w, h + 5 ), cvScalar( 0, 0, color ), 1 );
}
/*
void show_query_result(const char* query, BaseFeature** result, int nRes) {
    IplImage* query_img = cvLoadImage(query);
    if (!query_img)
        return;
    IplImage* query_gray = get_component_Y(query_img);
    show_query_result(query_gray, result, nRes);
}

void show_query_result(IplImage* query, BaseFeature** result, int nRes) {
    const int printW = 180;
    const int printH = 160;
    const int numOfRow = 3;

    int row = (nRes + numOfRow - 1) / numOfRow;
    
    int outputW = query->width + printW * numOfRow + 10 * numOfRow;
    int outputH = query->height > printH * row ? query->height : printH * row;

    IplImage *printImg = cvCreateImage(cvSize(outputW, outputH),
                                    IPL_DEPTH_8U, 1);

    cvZero(printImg);
    cvSetImageROI(printImg, cvRect(0, 0, query->width, query->height));
    cvAdd(printImg, query, printImg);


    IplImage *tmp = cvCreateImage(cvSize(printW, printH),
                                    IPL_DEPTH_8U, 1);
    for (int i = 0; i < nRes; i++) {
        IplImage *src = cvLoadImage(result[i]->get_name());
        IplImage *gray = get_component_Y(src);
        IplImage *push = NULL;
        if (gray->width > printW || gray->height > printH) {
            cvResize(gray, tmp);
            push = tmp;
        } else {
            push = gray;    
        }
        
        int w = (i % 3) * (printW + 10) + query->width + 10;
        int h = (i / 3) * printH;

        cvSetImageROI(printImg, cvRect(w, h, push->width, push->height));
        cvAdd(printImg, push, printImg);

        cvReleaseImage(&src);
        cvReleaseImage(&gray);
    }

    cvResetImageROI(printImg);
    cvReleaseImage(&tmp);
    cvReleaseImage(&query);

    show_image(printImg, "result");
    
}

void show_query_result(const uint8 *data, int w, int h,
        BaseFeature **result, int nRes) {
    if (nRes <= 0) {
        return ;
    }

    IplImage *query = yuv2iplImage(data, w, h);
    const int printW = 180;
    const int printH = 160;
    const int numOfRow = 3;

    int row = (nRes + numOfRow - 1) / numOfRow;
    
    int outputW = query->width + printW * numOfRow + 10 * numOfRow;
    int outputH = query->height > printH * row ? query->height : printH * row;

    IplImage *printImg = cvCreateImage(cvSize(outputW, outputH),
                                    IPL_DEPTH_8U, 1);

    cvZero(printImg);
    cvSetImageROI(printImg, cvRect(0, 0, query->width, query->height));
    cvAdd(printImg, query, printImg);


    IplImage *tmp = cvCreateImage(cvSize(printW, printH),
                                    IPL_DEPTH_8U, 1);
    for (int i = 0; i < nRes; i++) {
        IplImage *src = cvLoadImage(result[i]->get_name());
        IplImage *gray = get_component_Y(src);
        IplImage *push = NULL;
        if (gray->width > printW || gray->height > printH) {
            cvResize(gray, tmp);
            push = tmp;
        } else {
            push = gray;    
        }
        
        int w = (i % 3) * (printW + 10) + query->width + 10;
        int h = (i / 3) * printH;

        cvSetImageROI(printImg, cvRect(w, h, push->width, push->height));
        cvAdd(printImg, push, printImg);

        cvReleaseImage(&src);
        cvReleaseImage(&gray);
    }

    cvResetImageROI(printImg);
    cvReleaseImage(&tmp);
    cvReleaseImage(&query);

    show_image(printImg, "result");
} */

void show_yuv(const char *path) {
    FILE *pf = fopen(path, "r");
    if (pf == NULL) {
        fprintf(stderr, "Open Yuv Error!\n");
    }

    int w, h;
    fread(&w, sizeof(int), 1, pf);
    fread(&h, sizeof(int), 1, pf);

    uint8 *data = new uint8[w * h];
    fread(data, sizeof(uint8), w * h, pf);

    IplImage *img = yuv2iplImage(data, w, h);
    
    show_image(img, "yuv");

    cvReleaseImage(&img);
    delete [] data;
}

void show_yuv_colorful(const uint8 *data, int w, int h) {
    IplImage *mat = cvCreateImage(cvSize(w, h), IPL_DEPTH_8U, 3);
    vcd::cvt_YUV2RGB(data, w, h, mat);
    show_image(mat, "color_yuv");
    cvReleaseImage(&mat);
}

bool simulate_input(const char *path, uint8 *data, int kMaxImageSize,
                    int *w, int *h) {
    IplImage *src = cvLoadImage(path);
    if (src == NULL || src->nChannels != 3) {
        VLOG(-1, "Need Color Image %s", path);
        return false;
    }
    
    bool result = vcd::cvt_RGB2YUV(src, data, kMaxImageSize, w, h);
    cvReleaseImage(&src);
    return result;
}
