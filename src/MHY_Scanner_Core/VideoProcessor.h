#pragma once

#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

class VideoProcessor
{
public:
	AVCodecContext* avCodecContext = nullptr;
	const AVCodec* avCodec = nullptr;
	AVFormatContext* avformatContext = nullptr;
	AVCodecParameters* avCodecParameters = nullptr;
	AVPacket* avPacket = av_packet_alloc();;
	AVFrame* avframe = av_frame_alloc();
	AVFrame* pFrameBGR = av_frame_alloc();
	AVStream* avstream;
	struct SwsContext* swsCtx = 0;
public:
	int fps;
	int index;
	VideoProcessor();
	~VideoProcessor();

	int OpenVideo(std::string);
	int Close();
	int read();
	int GetStreamIndex(enum AVMediaType type);
	//int GetStream(AVCodecParameters* avCodecParameters, int streamIndex);

	int FFmpegDecoder(int);
	int OpenDecoder(int);
	int SendPacket();
	int ReceiveFrame();
	int buffer();// 创建帧缓冲区
	int swsctx(struct SwsContext** swsCtx);
private:


};
