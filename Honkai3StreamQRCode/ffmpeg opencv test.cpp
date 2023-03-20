#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

using namespace std;
using namespace cv;

int main(int argc, char* argv[])
{
    // 打开 FLV 视频文件
    avformat_network_init();
    AVFormatContext* format_ctx = avformat_alloc_context();
    if (avformat_open_input(&format_ctx, "test.flv", NULL, NULL) != 0) {
        cerr << "Error opening the file" << endl;
        return -1;
    }
    if (avformat_find_stream_info(format_ctx, NULL) < 0) {
        cerr << "Error finding the stream information" << endl;
        return -1;
    }
    int video_stream_index = -1;
    for (int i = 0; i < format_ctx->nb_streams; i++) {
        if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }
    if (video_stream_index == -1) {
        cerr << "Error finding the video stream" << endl;
        return -1;
    }
    AVCodecParameters* codec_par = format_ctx->streams[video_stream_index]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
    if (codec == NULL) {
        cerr << "Error finding the codec" << endl;
        return -1;
    }
    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (avcodec_parameters_to_context(codec_ctx, codec_par) != 0) {
        cerr << "Error copying codec parameters to context" << endl;
        return -1;
    }
    if (avcodec_open2(codec_ctx, codec, NULL) != 0) {
        cerr << "Error opening the codec" << endl;
        return -1;
    }

    // 创建窗口并初始化 OpenCV 图像处理
    namedWindow("FLV Video", cv::WINDOW_NORMAL);
    cv::namedWindow("FLV Video", cv::WINDOW_NORMAL);
    cv::resizeWindow("FLV Video", 1280, 720);
    
    AVPacket packet;
    av_init_packet(&packet);
    AVFrame* frame = av_frame_alloc();
    AVFrame* frame_BGR = av_frame_alloc();
    int num_bytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, codec_ctx->width, codec_ctx->height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(num_bytes * sizeof(uint8_t));
    av_image_fill_arrays(frame_BGR->data, frame_BGR->linesize, buffer, AV_PIX_FMT_BGR24, codec_ctx->width, codec_ctx->height, 1);
    SwsContext* sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, codec_ctx->pix_fmt, codec_ctx->width, codec_ctx->height, AV_PIX_FMT_BGR24, SWS_BILINEAR, NULL, NULL, NULL);

    // 循环读取视频帧并显示
    while (av_read_frame(format_ctx, &packet) >= 0) {
        if (packet.stream_index == video_stream_index) {
            avcodec_send_packet(codec_ctx, &packet);
            while (avcodec_receive_frame(codec_ctx, frame) == 0) {
                sws_scale(sws_ctx, frame->data, frame->linesize, 0, codec_ctx->height, frame_BGR->data, frame_BGR->linesize);
                Mat image(codec_ctx->height, codec_ctx->width, CV_8UC3, frame_BGR->data[0]);
                imshow("FLV Video", image);
                if (waitKey(1) == 27) {
                    break;
                }
            }
            av_packet_unref(&packet);
        }
    }
    // 释放资源
    av_free(buffer);
    av_frame_free(&frame);
    av_frame_free(&frame_BGR);
    avcodec_close(codec_ctx);
    avformat_close_input(&format_ctx);
    avformat_network_deinit();
    destroyAllWindows();

    return 0;
}
