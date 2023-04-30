#include "Scan.h"
//Video Snap
Scan::Scan()
{
	//av_log_set_level(AV_LOG_FATAL);
	avformatContext = avformat_alloc_context();
}

Scan::~Scan()
{
	if (avformatContext == avformat_alloc_context())
	{
		avformat_free_context(avformatContext);
		avformatContext = nullptr;
		avcodec_free_context(&avCodecContext);
		avCodecContext = nullptr;
	}
}

int Scan::OpenVideo(std::string path)
{
	if (avformatContext == nullptr)
	{
		return -1;
	}

	if (avformat_open_input(&avformatContext, path.c_str(), NULL, NULL) != 0)
	{
		std::cerr << "Error opening the file" << std::endl;
		return -1;
	}

	if (avformat_find_stream_info(avformatContext, NULL) < 0)
	{
		std::cerr << "Error finding the stream information" << std::endl;
		return -1;
	}

	return 0;
}

int Scan::Close()
{
	avformat_close_input(&avformatContext);
	return 0;
}

int Scan::read(AVPacket* avPacket)
{
	return av_read_frame(avformatContext, avPacket);
}

int Scan::GetStreamIndex(enum AVMediaType type)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(4500));
	return av_find_best_stream(avformatContext, type, -1, -1, nullptr, 0);
}

int Scan::FFmpegDecoder(int i) //获得视频解码器
{
	//avCodecContext = avcodec_alloc_context3(nullptr);
	//if (avcodec_parameters_to_context(avCodecContext, avCodecParameters) < 0)
	//{
	//	printf("error");
	//}
	avCodecParameters = avformatContext->streams[i]->codecpar;
	avCodec = avcodec_find_decoder(avCodecParameters->codec_id);
	return 0;
}

int Scan::OpenDecoder(int i)// 打开解码器
{
	avCodecContext = avcodec_alloc_context3(avCodec);
	avcodec_parameters_to_context(avCodecContext, avformatContext->streams[i]->codecpar);
	return avcodec_open2(avCodecContext, avCodec, nullptr);
}

int Scan::SendPacket(AVPacket avPacket)
{
	if (&avPacket == nullptr)
	{
		return avcodec_send_packet(avCodecContext, nullptr);
	}
	return avcodec_send_packet(avCodecContext, &avPacket);
}

int Scan::ReceiveFrame(AVFrame* avframe)
{
	return avcodec_receive_frame(avCodecContext, avframe);
}

int Scan::buffer(AVFrame* avframe)
{
	int numBytes = av_image_get_buffer_size(AV_PIX_FMT_BGR24, avCodecContext->width, avCodecContext->height, 1);
	uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));
	return av_image_fill_arrays(pFrameBGR->data, &pFrameBGR->linesize[0], buffer,
		AV_PIX_FMT_RGB24, avCodecContext->width, avCodecContext->height, 1);
}

int Scan::swsctx(struct SwsContext** swsCtx)
{
	*swsCtx = sws_getContext(avCodecContext->width, avCodecContext->height, avCodecContext->pix_fmt,
		avCodecContext->width, avCodecContext->height, AV_PIX_FMT_BGR24, SWS_BILINEAR, nullptr, nullptr, nullptr);
	return 0;
}