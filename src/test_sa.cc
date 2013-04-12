#include "utils.h"
#include "imitation.h"
#include "base/vcd_dir.h"
#include "feature/common.h"
#include "feature/img_saliency.h"
#include "base/logging.h"
#include "feature/om.h"
#include <stdio.h>
#include <dirent.h>
#include <highgui.h>
#include <vector>

const int LIB_NUM = 2;
const char *lib[] = {
                     "../img/",
                     "/mnt/db/sample/",
                     "/mnt/db/1/",
                     "/mnt/db/2/",
                     "/mnt/db/3/"};
const int VIDEO_LIB_NUM = 2;
const char *video_lib2[] = {"/mnt"};
const char *video_lib[] = {"/mnt/share/queryVideos",
                           "/mnt/share/libVideos/"};
const int kMaxSize = 1024 * 1024 * 3;
uint8 img_data[kMaxSize];
uint8 buff[1024 * 960 * 3];

int test_hc_method(const char *path, cv::Mat *res,
                    cv::Mat *result_, cv::Mat *result_RC_) {
    if (path == NULL) {
        return 1;
    }

    cv::Mat _src = cv::imread(path);
    if (!_src.data) {
        fprintf(stderr, "no data, %s\n", path);
        return 1;
    }

    cv::Mat img = _src;
    cv::Rect margin;
//    remove_margin(img, &margin);
    cv::Mat roi(img, margin);
    roi = img;
    cv::Mat src;
    src = roi;
    resize_mat_by_width(roi, src, 220);

    cv::Mat result;
    cv::Mat resultRC;
    cv::Rect rect;
    vcd::Saliency::Get(src, result, vcd::Saliency::HC);
    vcd::Saliency::Get(src, resultRC, vcd::Saliency::RC);
//    show_mat(result);
    vcd::Saliency::ExtractView(result, rect);
    //cv::Mat c[3] = {result, cv::Mat::zeros(),}
    draw_rectangle(result, rect);

    vcd::Saliency::ExtractView(resultRC, rect);
    draw_rectangle(resultRC, rect);

    //vcd::Saliency::Evaluate(result, result);
//
//    rect.x += margin.x;
//    rect.y += margin.y;
//
//    draw_rectangle(_src, rect);
//    //resize_mat_by_height(_src, _src, 160);
//    show_mat(_src);
//
//    char sname[128];
//    sprintf(sname, "%s_1.jpg", path);
//    imwrite(sname, src);
//    sprintf(sname, "%s_2.jpg", path);
//    imwrite(sname, resultRC);
//    show_mat(result);
//    if (!result.data) {
//        return;
//    }
    //show_mat(result);

    //*res = result;
    *result_ = result;
    *result_RC_ = resultRC;
    return 0;
}

int test_video_saliency(uint8 *data, int w, int h) {
    // 1= cvt the yuv data to rgb
    cv::Mat img_rgb;
    if (vcd::cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return 1;
    }

    cv::Mat img = img_rgb;
    cv::Rect margin;
    remove_margin(img, &margin);

    // 2= extract the main rectangle on rgb image
    cv::Mat roi(img, margin);
    //cv::Mat roi = img;
//    resize_mat_by_width(roi, roi, 320);
    cv::Rect rect;
    cv::Mat saliency_map;
    vcd::Saliency::Get(roi, saliency_map);    

    show_mat(saliency_map);
//    if (vcd::Saliency::ExtractView(saliency_map, rect) == false) {
//        return 1;
//    }
    
//    rect.x += margin.x;
//    rect.y += margin.y;
//
//    draw_rectangle(img, rect);
//    show_mat(img, 400);

//    char sname[128];
//    sprintf(sname, "%s_1.jpg");
//    imwrite(sname, img);
//    draw_rectangle(saliency_map, rect);
//    show_mat(saliency_map, 1000);

    return 0;
}

int idx = 0;
vcd::uint64 pre = 0;
int show_yuv_colorful_unconst(unsigned char *data, int w, int h) {
    cv::Mat img_rgb;
    if (vcd::cvt_YUV2RGB(data, w, h, &img_rgb) == false) {
        return 1;
    }
    //show_mat(img_rgb, 200);
    //show_yuv_colorful(data, w, h);

    resize_mat_by_width(img_rgb, img_rgb, 320);
//    vcd::uint64 cur = vcd::get_simply_feature(img_rgb);
//    if (cur != pre) {
//        pre = cur;
//    } else {
//        return 1;
//    }

    char name[127];
    sprintf(name, "/mnt/share/image_db/%d.jpg", idx++);
    cv::imwrite(name, img_rgb);

    return 0;
}

void test_dir2(const char *path) {
    vcd::Dir dir;
    std::string file_name;
    dir.OpenDir(path);
    
    while (1) {
        if (dir.GetNextFile(&file_name) == false)
            break;

        printf("%s\n", file_name.c_str());
        if (vcd::Imitation::ProcessVideo(file_name.c_str(),
                                         show_yuv_colorful_unconst) != 0)
            continue;
        break;
    }
}

void test_dir(const char *path) {
    vcd::Imitation imi;
    imi.OpenJpgDb(path);
    imi.GetVideo(show_yuv_colorful_unconst);
}

void test_yuv(const char *path) {
    vcd::Imitation imi;
    imi.OpenJpgDb(path);
    imi.GetNextYUV(test_video_saliency);
}

void test_jpg_dir(const char *path) {
    vcd::Dir dir;
    dir.OpenDir(path);
    std::string filename;
    cv::Mat res, resRc;
    std::vector<cv::Mat> hc_sa;
    std::vector<cv::Mat> rc_sa;
    while (1) {
        if (dir.GetNextFile(&filename) == false) {
            break;
        }

        if (test_hc_method(filename.c_str(), NULL, &res, &resRc) != 0) 
            continue;
        hc_sa.push_back(res.clone());
        rc_sa.push_back(resRc.clone());
    }

    int big_w = hc_sa[0].cols * 2 + 10;
    int big_h = -5;
    for (int i = 0; i < hc_sa.size(); ++i) {
        big_h += hc_sa[i].rows + 5;
    }

    cv::Mat result(cv::Size(big_w, big_h), CV_8UC1, cv::Scalar(255,255,255));
    int top = 0;
    for (int i = 0; i < hc_sa.size(); ++i) {
        cv::Mat roi_hc(result, cv::Rect(0, top, hc_sa[i].cols, hc_sa[i].rows));
        cv::Mat roi_rc(result, cv::Rect(hc_sa[i].cols + 10, top,
                                        hc_sa[i].cols, hc_sa[i].rows));

        //roi_hc = cv::Scalar(i *50,i * 50, i*50);
        //roi_rc = cv::Scalar(i* 15);
        //hc_sa[i].copyTo(result);
        //rc_sa[i].copyTo(roi_rc);
        for (int r = 0; r < hc_sa[i].rows; ++r) {
            for (int c = 0; c < hc_sa[i].cols; ++c) {
                for (int k = 0; k < 1; ++k) {
                    roi_hc.at<uint8>(r,c) = hc_sa[i].at<uint8>(r,c);
                    roi_rc.at<uint8>(r,c) = rc_sa[i].at<uint8>(r,c);
                }
            }
        }

//        for (int j = 0; j < hc_sa[i].rows; ++j)
//        roi_hc.row(j) = hc_sa[i].row(j);

        top += hc_sa[i].rows + 5;
//        show_mat(result);
//        show_mat(roi_hc);
    }
    show_mat(result);
    cv::imwrite("/mnt/share/big_result_view.jpg", result);
}

void test_single_pic(const char *path) {
    cv::Mat mat;
    //test_hc_method(path, &mat);
    show_mat(mat);
}

int main(int argc, char **argv) {
    //test_yuv(argv[1]);
    //test_jpg_dir(argv[1]);
    test_dir2(argv[1]);
//    test_single_pic("../img/1.jpg");
//    for (int i = 0; i < LIB_NUM; ++i) 
//        test_dir(lib[i]);
//    for (int i = 0; i < VIDEO_LIB_NUM; ++i) {
//        test_dir(video_lib[i]);
//    }
}
