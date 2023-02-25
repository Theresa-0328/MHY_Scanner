#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <zbar.h>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
}

class Scan
{
public:
	std::string path;
	//int numBytes = 0;
	AVCodecContext* avCodecContext = nullptr;
	const AVCodec* avCodec = nullptr;
	AVFormatContext* avformatContext = nullptr;
	AVCodecParameters* avCodecParameters = nullptr;
	AVPacket* avPacket = av_packet_alloc();;
	AVFrame* avframe = av_frame_alloc();
	AVFrame* pFrameBGR = av_frame_alloc();
	struct SwsContext* swsCtx = 0;
public:
	Scan();
	~Scan();

	int OpenVideo(std::string);
	int Close();
	int read(AVPacket* packet);
	int GetStreamIndex(enum AVMediaType type);
	//int GetStream(AVCodecParameters* avCodecParameters, int streamIndex);

	int FFmpegDecoder(int);
	int OpenDecoder(int);
	int SendPacket(AVPacket avPacket);
	int ReceiveFrame(AVFrame* avframe);
	int buffer(AVFrame* pFrameBGR);// ´´½¨Ö¡»º³åÇø
	int swsctx(struct SwsContext** swsCtx);
private:


};
