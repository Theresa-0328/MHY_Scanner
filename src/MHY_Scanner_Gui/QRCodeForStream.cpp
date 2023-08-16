#include "QRCodeForStream.h"

#include "OfficialApi.h"
#include "Mihoyosdk.h"
#include "ThreadSacn.h"

ThreadStreamProcess::ThreadStreamProcess(QObject* parent)
	: QThread(parent),
	pAvdictionary(nullptr),
	pAVFormatContext(nullptr),
	pSwsContext(nullptr),
	pAVFrame(nullptr),
	pAVPacket(nullptr),
	pAVCodecContext(nullptr)
{
	av_log_set_level(AV_LOG_FATAL);
}

ThreadStreamProcess::~ThreadStreamProcess()
{
	if (!this->isInterruptionRequested())
	{
		QMutexLocker lock(&m_mux);
		m_stop = false;
	}
	this->requestInterruption();
	this->wait();
}

void ThreadStreamProcess::setLoginInfo(std::string uid, std::string gameToken)
{
	this->uid = uid;
	this->gameToken = gameToken;
}

void ThreadStreamProcess::setLoginInfo(const std::string uid, const std::string gameToken, const std::string name)
{
	this->uid = uid;
	this->gameToken = gameToken;
	this->m_name = name;
}


void ThreadStreamProcess::setServerType(const ServerType::Type& servertype)
{
	this->servertype = servertype;
}

void ThreadStreamProcess::LoginOfficial()
{
	OfficialApi o;
	ThreadSacn threadsacn;
	const std::string& uuid = o.generateUUID();
	threadsacn.start();
	while (m_stop)
	{
		pAVPacket = av_packet_alloc();
		pAVFrame = av_frame_alloc();
		int code = av_read_frame(pAVFormatContext, pAVPacket);
		if (code < 0)
		{
			return;
		}
		avcodec_send_packet(pAVCodecContext, pAVPacket);
		if (pAVPacket->stream_index != videoStremIndex)
		{
			continue;
		}
		if (pAVFrame == nullptr)
		{
			std::cerr << "Error allocating frame" << std::endl;
			break;
		}
		if (avcodec_receive_frame(pAVCodecContext, pAVFrame) != 0)
		{
			continue;
		}
		cv::Mat img(720, 1280, CV_8UC3);
		uint8_t* dstData[1] = { img.data };
		int dstLinesize[1] = { static_cast<int>(img.step) };
		sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height, dstData, dstLinesize);
		if (threadsacn.MatEmpty())
		{
			threadsacn.setImg(img);
		}
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Honkai3);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		if (threadsacn.getQRcode().find("biz_key=hk4e_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Genshin);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		if (threadsacn.getQRcode().find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::StarRail);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		av_frame_free(&pAVFrame);
		av_packet_unref(pAVPacket);
	}
	threadsacn.stop();
}

void ThreadStreamProcess::LoginBH3BiliBili()
{
	Mihoyosdk m;
	const std::string LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);
	ThreadSacn threadsacn;
	threadsacn.start();
	while (m_stop)
	{
		pAVPacket = av_packet_alloc();
		pAVFrame = av_frame_alloc();
		int code = av_read_frame(pAVFormatContext, pAVPacket);
		if (code < 0)
		{
			return;
		}
		avcodec_send_packet(pAVCodecContext, pAVPacket);
		if (pAVPacket->stream_index != videoStremIndex)
		{
			continue;
		}
		if (pAVFrame == nullptr)
		{
			std::cerr << "Error allocating frame" << std::endl;
			break;
		}
		if (avcodec_receive_frame(pAVCodecContext, pAVFrame) != 0)
		{
			continue;
		}
		cv::Mat img(720, 1280, CV_8UC3);
		uint8_t* dstData[1] = { img.data };
		int dstLinesize[1] = { static_cast<int>(img.step) };
		sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height, dstData, dstLinesize);
		if (threadsacn.MatEmpty())
		{
			threadsacn.setImg(img);
		}
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			break;
		}
		av_frame_free(&pAVFrame);
		av_packet_unref(pAVPacket);
	}
	threadsacn.stop();
}

//void ThreadStreamProcess::LoginOfficial()
//{
//	OfficialApi o;
//	std::string uuid = o.generateUUID();
//	do
//	{
//
//	} while (false);
//	ThreadSacn threadsacn;
//	VideoProcessor vp;
//	QThread::msleep(5000);
//	//错误判断
//	if (vp.OpenVideo("./Cache/output.flv") != 0)
//	{
//	}
//	int64_t latestTimestamp = av_gettime_relative();
//	//if (vp.avformatContext->streams[vp.index]->start_time != AV_NOPTS_VALUE)
//	//{
//	//	int64_t streamTimestamp = av_rescale_q(vp.avformatContext->streams[vp.index]->start_time,
//	//		vp.avformatContext->streams[vp.index]->time_base, { 1, AV_TIME_BASE });
//	//	if (streamTimestamp > latestTimestamp)
//	//	{
//	//		latestTimestamp = streamTimestamp;
//	//	}
//	//}
//	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
//	int f = 0;
//#ifdef _DEBUG
//	cv::namedWindow("Video");
//#endif // _DEBUG
//	while (true)
//	{
//		//int64_t video_duration_sec = vp.avformatContext->duration / AV_TIME_BASE;
//		//size_t timeInSeconds = av_q2d(vp.avstream->time_base);
//		if (stopStream)
//		{
//#ifdef _DEBUG
//			cv::destroyWindow("Video");
//#endif // _DEBUG
//			break;
//		}
//		int op1 = vp.read();
//		if (vp.avPacket->stream_index != vp.index)
//		{
//			continue;
//		}
//		if (vp.SendPacket() != 0)
//		{
//			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
//			QThread::msleep(300);
//		}
//		vp.ReceiveFrame();
//		//转换像素格式
//		sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
//			vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
//		cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
//		//NOTE:可能有1280 1980和60帧 30帧,二维码不一定在屏幕的正中央。
//		//cv::Mat crop_img = img(cv::Range(300, 800), cv::Range(500, 1186));
//		cv::Mat _img = img(cv::Rect(0, 0, 1280, 720));
//
//		//cv::Mat _img(720, 1280, CV_8UC3);
//		//uint8_t* dstData[1] = { _img.data };
//		//int dstLinesize[1] = { _img.step };
//		//int code = sws_scale(vp.swsCtx, vp.pFrameBGR->data, vp.pFrameBGR->linesize, 0, vp.avCodecContext->height, dstData, dstLinesize);
//
//		if (!threadsacn.isRunning())
//		{
//			threadsacn.setImg(_img);
//			threadsacn.start();
//			//#ifdef _DEBUG
//			std::cout << "scan count " << f++ << std::endl;
//			cv::imshow("Video", _img);
//			//#endif
//			cv::waitKey(2);
//		}
//		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
//		{
//			o.setGameType(GameType::Type::Honkai3);
//			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
//			emit loginResults(code == 0);
//			continue;
//		}
//		//TODO:Genshin
//		if (threadsacn.getQRcode().find("biz_key=hkrpg_cn") != std::string::npos)
//		{
//			o.setGameType(GameType::Type::StarRail);
//			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
//			emit loginResults(code == 0);
//			continue;
//		}
//		av_packet_unref(vp.avPacket);
//	}
//}

//void ThreadStreamProcess::LoginBH3BiliBili()
//{
//	do
//	{
//
//	} while (false);
//	Mihoyosdk m;
//	const std::string LoginData = m.verify(uid, gameToken);
//	m.setUserName(m_name);
//	ThreadSacn threadsacn;
//	QThread::msleep(5000);
//	VideoProcessor vp;
//	//错误判断
//	if (vp.OpenVideo("./Cache/output.flv") != 0)
//	{
//		throw std::logic_error("parse number error");
//	}
//	int64_t latestTimestamp = av_gettime_relative();
//	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
//	int f = 0;
//#ifdef _DEBUG
//	cv::namedWindow("Video");
//#endif // _DEBUG
//	while (true)
//	{
//		if (m_stop)
//		{
//#ifdef _DEBUG
//			cv::destroyWindow("Video");
//#endif // _DEBUG
//			break;
//		}
//		int op1 = vp.read();
//		if (vp.avPacket->stream_index != vp.index)
//		{
//			continue;
//		}
//		if (vp.SendPacket() != 0)
//		{
//			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
//			QThread::msleep(300);
//		}
//		vp.ReceiveFrame();
//		// 转换像素格式
//		sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
//			vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
//		cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
//		cv::Mat _img = img(cv::Rect(0, 0, 1280, 720));
//		if (!threadsacn.isRunning())
//		{
//			threadsacn.setImg(_img);
//			threadsacn.start();
//			//#ifdef _DEBUG
//			std::cout << "scan count " << f++ << std::endl;
//			imshow("Video", _img);
//			//#endif
//			cv::waitKey(2);
//		}
//		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
//		{
//			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
//			emit loginResults(code == 0);
//			continue;
//		}
//		av_packet_unref(vp.avPacket);
//	}
//}

void ThreadStreamProcess::stop()
{
	QMutexLocker lock(&m_mux);
	m_stop = false;
}

void ThreadStreamProcess::setUrl(const std::string& url, const std::map<std::string, std::string>& heard)
{
	m_url = url;
	for (const auto& it : heard)
	{
		av_dict_set(&pAvdictionary, it.first.c_str(), it.second.c_str(), 0);
	}
}

bool ThreadStreamProcess::init()
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
	return true;
}

void ThreadStreamProcess::run()
{
	m_stop = true;
	//TODO 获取直播流地址放在这里
	bool code = init();
	switch (servertype)
	{
	case ServerType::Official:
		LoginOfficial();
		break;
	case ServerType::BH3_BiliBili:
		LoginBH3BiliBili();
		break;
	default:
		break;
	}
	avformat_close_input(&pAVFormatContext);
	avcodec_free_context(&pAVCodecContext);
	sws_freeContext(pSwsContext);
	av_dict_free(&pAvdictionary);
}