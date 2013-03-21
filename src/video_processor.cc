#include "video_processor.h"
#include "utils.h"
#include <stdio.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}
namespace vcd {

bool get_next_frame(AVFormatContext *pformat_ctx, AVCodecContext *pcodec_ctx,
                    int video_stream, AVFrame *pframe) {
    static AVPacket packet;

    int frame_finished = 0;
    while (true) {
        // read next frame. if no one, return;
        if (av_read_frame(pformat_ctx, &packet) < 0) {
            fprintf(stderr, "read frame error!\n");
            return false;
        }

        // check the video stream index
        if (packet.stream_index != video_stream) {
            goto next_loop;
        }

        // decode this frame
        avcodec_decode_video2(pcodec_ctx, pframe, &frame_finished, &packet);

next_loop:
        av_free_packet(&packet);
        
        // if one frame is decoded, return back
        if (frame_finished) {
            break;
        }
    }

    return true;
}

int read_video(const char *path, process_func process) {
    // Register all formats and codecs
    av_register_all();

    // open video file
    AVFormatContext *pformat_ctx = avformat_alloc_context();
    if (avformat_open_input(&pformat_ctx, path, NULL, NULL) != 0) {
        fprintf(stderr, "open video %s error!\n", path);
        return -1;      // open error!
    }

    // retrieve stream infor
    if (avformat_find_stream_info(pformat_ctx, NULL) < 0) {
        return -1;
    }

    av_dump_format(pformat_ctx, 0, path, false);

    // find the first video stream
    int video_stream = -1;
    for (size_t i = 0; i < pformat_ctx->nb_streams; ++i) {
        if (pformat_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
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

    if (pcodec->capabilities & CODEC_CAP_TRUNCATED) {
        pcodec_ctx->flags |= CODEC_FLAG_TRUNCATED;
    }
    // open codec
    if (avcodec_open2(pcodec_ctx, pcodec, NULL) < 0) {
        return -1;
    }

    // allocate video frame
    AVFrame *pframe, *pframe_rgb;
    pframe = avcodec_alloc_frame();
    pframe_rgb = avcodec_alloc_frame();

    if (pframe_rgb == NULL) return -1;

    int num_bytes = avpicture_get_size(PIX_FMT_YUV420P, pcodec_ctx->width,
                                       pcodec_ctx->height);
    uint8 *buffer = new uint8[num_bytes];
    avpicture_fill((AVPicture*)pframe_rgb, buffer, PIX_FMT_YUV420P,
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
        int offset = 0;
        int width = pcodec_ctx->width;
        int height = pcodec_ctx->height;
        if ((i++) % 20 != 0) {
            continue;
        }
//        printf("%d\n", i++);

        for (int c = 0; c < 3; ++c) {
            uint8_t *ptr = pframe->data[c];
            int linesize = pframe->linesize[c];
            if (c == 1) {
                width >>= 1;
                height >>= 1;
            }
            for (int j = 0; j < height; ++j) {
                memcpy(buffer + offset, ptr, width * sizeof(uint8_t));
                ptr += linesize;
                offset += width;
            }
        }
        //show_yuv_colorful(buffer, width * 2, height * 2);
//        show_yuv(buffer, width * 2, height * 2);
        (*process)(buffer, width * 2, height * 2);
    }

    delete [] buffer;
    av_free(pframe);
    av_free(pframe_rgb);

    avcodec_close(pcodec_ctx);
    avformat_close_input(&pformat_ctx);
    
    return 0;
}

VideoProcessor::VideoProcessor() {
}

VideoProcessor::~VideoProcessor() {
}

bool VideoProcessor::ProcessVideo(const char *path, process_func f) {
    read_video(path, f);
    return true;
}

} // namespace vcd

//int main(int argc, char **argv) {
//    if (argc != 2) {
//        return 1;
//    }
//    vcd::read_video(argv[1]);
//
//    return 0;
//}
