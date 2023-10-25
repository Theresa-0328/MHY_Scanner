#include <iostream>
#include <string>
#include <Windows.h>
extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
};
#include <opencv2/opencv.hpp>
#include <QThread>
#include <QMutex>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include "LiveStreamLink.h"
#include "QRScanner.h"

#if 0
class QRCodeForStream :public QThread
{
public:
	QRCodeForStream() = default;
	void setp(const cv::Mat& m_i)
	{
		if (m_mux.tryLock())
		{
			this->m_i = m_i;
			m_mux.unlock();
		}
	}
	bool MatEmpty()const
	{
		return m_i.empty();
	}
	void run()
	{
		QRScanner qrscanner;
		m_stop = true;
		while (m_stop)
		{
			if (!MatEmpty())
			{
				cv::imshow("test1", m_i);
				qrscanner.decodeMultiple(m_i, m_qr);
				if (!m_qr.empty())
					std::cout << "qr:" << m_qr << std::endl;
				m_i.release();
			}
			cv::waitKey(10);
		}
	}
	void SetStop(bool stop)
	{
		m_stop = stop;
	}
	~QRCodeForStream() = default;
private:
	QMutex m_mux;
	cv::Mat m_i;
	std::string m_qr;
	bool m_stop = false;
};
#endif

class AvFFmpegDecoder :public QThread
{
public:
	AvFFmpegDecoder(QObject* parent = nullptr);
	~AvFFmpegDecoder();
	void SetUrl(const std::string& url, const std::map<std::string, std::string>& heard = {});
	bool Init();
	void run();

private:
	std::string m_url;
	AVDictionary* pAvdictionary;
	AVFormatContext* pAVFormatContext;
	AVCodecContext* pAVCodecContext;
	SwsContext* pSwsContext;
	AVFrame* pAVFrame;
	AVPacket* pAVPacket;
	AVIOContext* pAVIOContext;
	int videoStremIndex = 0;
	bool m_stop = false;

};

AvFFmpegDecoder::AvFFmpegDecoder(QObject* parent)
	:QThread(parent),
	pAvdictionary(nullptr),
	pAVFormatContext(nullptr),
	pSwsContext(nullptr),
	pAVFrame(nullptr),
	pAVPacket(nullptr),
	pAVCodecContext(nullptr)
{
	//av_log_set_level(AV_LOG_FATAL);
}

AvFFmpegDecoder::~AvFFmpegDecoder()
{
	m_stop = false;
	avformat_close_input(&pAVFormatContext);
	avcodec_free_context(&pAVCodecContext);
	sws_freeContext(pSwsContext);
	av_dict_free(&pAvdictionary);
	av_frame_free(&pAVFrame);
	av_packet_free(&pAVPacket);
}

bool AvFFmpegDecoder::Init()
{
	if (avformat_open_input(&pAVFormatContext, m_url.c_str(), NULL, &pAvdictionary) != 0)
	{
		std::cerr << "Error opening input file" << std::endl;
		return false;
	}

	if (avformat_find_stream_info(pAVFormatContext, NULL) < 0)
	{
		std::cerr << "Error finding stream information" << std::endl;
		return false;
	}
	AVStream* videoStream = nullptr;
	for (int i = 0; i < pAVFormatContext->nb_streams; i++)
	{
		if (pAVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoStream = pAVFormatContext->streams[i];
			break;
		}
	}
	if (videoStream == nullptr)
	{
		std::cerr << "No video stream found" << std::endl;
		return false;
	}
	videoStremIndex = videoStream->index;
	const AVCodec* decoder = avcodec_find_decoder(videoStream->codecpar->codec_id);
	if (decoder == nullptr)
	{
		std::cerr << "Codec not found" << std::endl;
		return false;
	}
	pAVCodecContext = avcodec_alloc_context3(decoder);
	avcodec_parameters_to_context(pAVCodecContext, videoStream->codecpar);
	if (avcodec_open2(pAVCodecContext, decoder, NULL) < 0)
	{
		std::cerr << "Error opening codec" << std::endl;
		return false;
	}
	pSwsContext = sws_getContext(
		pAVCodecContext->width, pAVCodecContext->height, pAVCodecContext->pix_fmt,
		1280, 720, AV_PIX_FMT_BGR24,
		SWS_BILINEAR, NULL, NULL, NULL
	);
}

void AvFFmpegDecoder::SetUrl(const std::string& url, const std::map<std::string, std::string>& heards)
{
	m_url = url;
	for (const auto& it : heards)
	{
		av_dict_set(&pAvdictionary, it.first.c_str(), it.second.c_str(), 0);
	}
}

void AvFFmpegDecoder::run()
{
	//QRCodeForStream c;
	//c.start();
	Init();
	pAVPacket = av_packet_alloc();
	pAVFrame = av_frame_alloc();
	m_stop = true;
	while (m_stop)
	{
		int code = av_read_frame(pAVFormatContext, pAVPacket);
		if (code < 0)
		{
			return;
		}
		avcodec_send_packet(pAVCodecContext, pAVPacket);
		if (pAVPacket->stream_index != videoStremIndex)
		{
			av_packet_unref(pAVPacket);
			continue;
		}
		if (pAVFrame == nullptr)
		{
			std::cerr << "Error allocating frame" << std::endl;
			break;
		}
		if (avcodec_receive_frame(pAVCodecContext, pAVFrame) != 0)
		{
			av_frame_unref(pAVFrame);
			continue;
		}
		cv::Mat outputFrame(720, 1280, CV_8UC3);
		uint8_t* dstData[1] = { outputFrame.data };
		int dstLinesize[1] = { outputFrame.step };
		sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height, dstData, dstLinesize);
		//if (c.MatEmpty())
		//	c.setp(outputFrame);

		cv::imshow("Video Player", outputFrame);
		cv::waitKey(1);

		av_frame_unref(pAVFrame);
		av_packet_unref(pAVPacket);
	}

}

void main_leak_test()
{
	//LiveHuya l("353322");
	//LiveBili l("6");
	LiveDouyin l("780224766141");
	l.GetLiveStreamStatus();
	std::string str = l.GetLiveStreamLink();
	std::map<std::string, std::string> heards =
	{
		{"user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) \
			Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41"},
		{"referer", "https://live.bilibili.com"}
	};
	AvFFmpegDecoder a;
	//std::string str;
	//a.SetUrl(str, heards);
	a.SetUrl(str);
	std::cout << str << std::endl;
	a.start();
}

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
	main_leak_test();
	return 0;
}