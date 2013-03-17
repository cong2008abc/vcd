#include "video_processor.h"
#include <stdio.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
namespace vcd {

bool get_next_frame(AVFormatContext *pframe_ctx, AVCodecContext *pcodec_ctx,
                    int video_stream, AVFrame *pframe) {
    // ...
}

int read_video(const char *path) {
    AVFormatContext *pformat_ctx;

    // Register all formats and codecs
    av_register_all();

    // open video file
    if (av_open_input_file(&pformat_ctx, path, NULL, 0, NULL) != 0) {
        return -1;      // open error!
    }

    // retrieve stream infor
    if (av_find_stream_info(pformat_ctx) < 0) {
        return -1;
    }

    // dump_format(pformat_ctx, 0, path, false);

    // find the first video stream
    int video_stream = -1;
    for (int i = 0; i < pformat_ctx->nb_streams; ++i) {
        if (pformat_ctx->streams[i]->codec->codec_type == CODEC_TYPE_VIDEO) {
            video_stream = i;
            break;
        }
    }
    if (video_stream == -1) {
        return -1;
    }

    // get a pointer to the codec context for the video stream
    AVCodecContext *pcodec_ctx;
    pcodec_ctx = pformat_ctx->streams[video_stream]->codec;

    // find the decoder for the video stream
    AVCodec *pcodec = avcodec_find_decoder(pcodec_ctx->codec_id);
    if (pcodec == NULL) {
        return -1;
    }

    // ??
    if (pcodec->capabilities & CODEC_CAP_TRUNCATED) {
        pcodec_ctx->flags |= CODEC_FLAG_TRUNCATED;
    }

    // open codec
    if (avcodec_open(pcodec_ctx, pcodec) < 0) {
        return -1;
    }

//    if (pcodec_ctx->frame_rate > 1000 && pcodec_ctx->frame_rate_base == 1) {
//        pcodec_ctx->frame_rate_base = 1000;
//    }

    // allocate video frame
    AVFrame *pframe, *pframe_rgb;
    pframe = avcodec_alloc_frame();
    pframe_rgb = avcodec_alloc_frame();

    if (pframe_rgb == NULL) return -1;

    int num_bytes = avpicture_get_size(PIX_FMT_RGB24, pcodec_ctx->width,
                                       pcodec_ctx->height);
    uint8 *buffer = new uint8[num_bytes];
    avpicture_fill((AVPicture*)pframe_rgb, buffer, PIX_FMT_RGB24,
                   pcodec_ctx->width, pcodec_ctx->height);
    SwsContext *img_cvt_ctx;
    img_cvt_ctx = sws_getContext(pcodec_ctx->width, pcodec_ctx->height, pcodec_ctx->pix_fmt,
                                 pcodec_ctx->width, pcodec_ctx->height, PIX_FMT_YUV420P,
                                 SWS_BICUBIC, NULL, NULL, NULL);
    if (img_cvt_ctx == NULL) {
        fprintf(stderr, "Cannot initialize the conversion context!\n");
        return -1;
    }
    int i = 0;
    while (get_next_frame(pformat_ctx, pcodec_ctx, video_stream, pframe)) {
//        img_convert((AVPicture*)pframe_rgb, PIX_FMT_RGB24, (AVPicture*)pframe,
//                    pcodec_ctx->pix_fmt, pcodec_ctx->width, pcodec_ctx->height);
//        sws_scale(img_cvt_ctx, pframe->data, pframe->linesize, 0, pcodec_ctx->height,
                

        // do somthing!.
    }

    delete [] buffer;
    av_free(pframe);
    av_free(pframe_rgb);

    avcodec_close(pcodec_ctx);
    av_close_input_file(pformat_ctx);
    
    return 0;
}

} // namespace vcd

int main() {
    
}
