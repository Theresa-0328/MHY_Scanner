#include "QRCodeForStream.h"

#include <string>
#include <string_view>

#include "QRScanner.h"

QRCodeForStream::QRCodeForStream(QObject* parent) :
    QThread(parent),
    pAvdictionary(nullptr),
    pAVFormatContext(nullptr),
    pSwsContext(nullptr),
    pAVFrame(nullptr),
    pAVPacket(nullptr),
    pAVCodecContext(nullptr),
    m_stop(false),
    servertype(ServerType::Official)

{
    av_log_set_level(AV_LOG_FATAL);
    m_config = &(ConfigDate::getInstance());
}

QRCodeForStream::~QRCodeForStream()
{
    if (!this->isInterruptionRequested())
    {
        m_stop.store(false);
    }
    this->requestInterruption();
    this->wait();
}

void QRCodeForStream::setLoginInfo(const std::string& uid, const std::string& gameToken)
{
    this->uid = uid;
    this->gameToken = gameToken;
}

void QRCodeForStream::setLoginInfo(const std::string& uid, const std::string& gameToken, const std::string& name)
{
    this->uid = uid;
    this->gameToken = gameToken;
    this->m_name = name;
}

void QRCodeForStream::setServerType(const ServerType servertype)
{
    this->servertype = servertype;
}

void QRCodeForStream::LoginOfficial()
{
    std::mutex mtx;
    while (m_stop.load())
    {
        if (av_read_frame(pAVFormatContext, pAVPacket) < 0)
        {
            ret = ScanRet::Type::LIVESTOP;
            break;
        }
        if (pAVPacket->stream_index != videoStreamIndex)
        {
            continue;
        }
        avcodec_send_packet(pAVCodecContext, pAVPacket);
        if (pAVFrame == nullptr)
        {
            std::cerr << "Error allocating frame" << std::endl;
            ret = ScanRet::Type::LIVESTOP;
            break;
        }
        while (avcodec_receive_frame(pAVCodecContext, pAVFrame) == 0)
        {
            cv::Mat img(videoStreamHeight, videoStreamWidth, CV_8UC3);
            uint8_t* dstData[1] = { img.data };
            const int dstLinesize[1] = { static_cast<int>(img.step) };
            sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height,
                      dstData, dstLinesize);
#ifndef SHOW
            cv::imshow("Video_Stream", img);
            cv::waitKey(1);
#endif
            threadPool.tryStart([&, img = std::move(img)]() {
                thread_local QRScanner qrScanners;
                std::string str;
                qrScanners.decodeSingle(img, str);
                if (str.size() < 85)
                {
                    return;
                }
                std::string_view view(str.c_str() + 79, 3);
                if (setGameType.count(view) == 0)
                {
                    return;
                }
                setGameType[view]();
                const std::string& ticket = str.substr(str.length() - 24);
                if (o.validityCheck(ticket))
                {
                    return;
                }
                if (mtx.try_lock())
                {
                    if (!m_stop.load())
                    {
                        mtx.unlock();
                        return;
                    }
                    o.scanInit(m_gametype, ticket, uid, gameToken);
                    ret = o.scanRequest();
                    if (ret == ScanRet::Type::SUCCESS)
                    {
                        json::Json config;
                        config.parse(m_config->getConfig());
                        if (config["auto_login"])
                        {
                            ret = o.scanConfirm();
                            emit loginResults(ret);
                        }
                        else
                        {
                            emit loginConfirm(m_gametype, false);
                        }
                    }
                    else
                    {
                        emit loginResults(ret);
                    }
                    stop();
                    mtx.unlock();
                }
            });
        }
        av_frame_unref(pAVFrame);
        av_packet_unref(pAVPacket);
    }
}

void QRCodeForStream::LoginBH3BiliBili()
{
    const std::string& LoginData = m.verify(uid, gameToken);
    m.setUserName(m_name);
    std::mutex mtx;
    while (m_stop.load())
    {
        if (av_read_frame(pAVFormatContext, pAVPacket) < 0)
        {
            ret = ScanRet::Type::LIVESTOP;
            break;
        }
        if (pAVPacket->stream_index != videoStreamIndex)
        {
            continue;
        }
        avcodec_send_packet(pAVCodecContext, pAVPacket);
        if (pAVFrame == nullptr)
        {
            std::cerr << "Error allocating frame" << std::endl;
            ret = ScanRet::Type::LIVESTOP;
            break;
        }

        while (avcodec_receive_frame(pAVCodecContext, pAVFrame) == 0)
        {
            cv::Mat img(videoStreamHeight, videoStreamWidth, CV_8UC3);
            uint8_t* dstData[1] = { img.data };
            const int dstLinesize[1] = { static_cast<int>(img.step) };
            sws_scale(pSwsContext, pAVFrame->data, pAVFrame->linesize, 0, pAVFrame->height,
                      dstData, dstLinesize);
#ifndef SHOW
            cv::imshow("Video_Stream", img);
            cv::waitKey(1);
#endif
            threadPool.tryStart([&, img = std::move(img)]() {
                thread_local QRScanner qrScanners;
                std::string str;
                qrScanners.decodeSingle(img, str);
                if (str.size() < 85)
                {
                    return;
                }
                if (std::string_view view(str.c_str() + 79, 3); view != "8F3")
                {
                    return;
                }
                const std::string& ticket = str.substr(str.length() - 24);
                if (m.validityCheck(ticket))
                {
                    return;
                }
                if (mtx.try_lock())
                {
                    if (!m_stop.load())
                    {
                        mtx.unlock();
                        return;
                    }
                    m.scanInit(ticket, LoginData);
                    if (ret = m.scanCheck(); ret == ScanRet::Type::SUCCESS)
                    {
                        json::Json config;
                        config.parse(m_config->getConfig());
                        if (config["auto_login"])
                        {
                            ret = m.scanConfirm();
                            emit loginResults(ret);
                        }
                        else
                        {
                            emit loginConfirm(GameType::Type::Honkai3_BiliBili, false);
                        }
                    }
                    else
                    {
                        emit loginResults(ret);
                    }
                    stop();
                    mtx.unlock();
                }
            });
        }
        av_frame_unref(pAVFrame);
        av_packet_unref(pAVPacket);
    }
}

void QRCodeForStream::setStreamHW()
{
    if (pAVCodecContext->width < pAVCodecContext->height ||
        pAVCodecContext->height == 480 ||
        pAVCodecContext->height == 720)
    {
        videoStreamWidth = pAVCodecContext->width;
        videoStreamHeight = pAVCodecContext->height;
    }
    else
    {
        videoStreamWidth = pAVCodecContext->width / 1.5;
        videoStreamHeight = pAVCodecContext->height / 1.5;
    }
}

void QRCodeForStream::stop()
{
    m_stop.store(false);
}

void QRCodeForStream::setUrl(const std::string& url, const std::map<std::string, std::string> heard)
{
    m_url = url;
    for (const auto& it : heard)
    {
        av_dict_set(&pAvdictionary, it.first.c_str(), it.second.c_str(), 0);
    }
    av_dict_set(&pAvdictionary, "max_delay", "0", 0);
    av_dict_set(&pAvdictionary, "probesize", "1024", 0);
    av_dict_set(&pAvdictionary, "packetsize", "128", 0);
    av_dict_set(&pAvdictionary, "rtbufsize", "0", 0);
    av_dict_set(&pAvdictionary, "delay", "0", 0);
    av_dict_set(&pAvdictionary, "buffer_size", "1000", 0);
}

auto QRCodeForStream::init() -> bool
{
    pAVFormatContext = avformat_alloc_context();
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
    videoStreamIndex = videoStream->index;
    //const AVCodec* decoder{ avcodec_find_decoder_by_name("h264_cuvid") };
    const AVCodec* decoder{ avcodec_find_decoder(videoStream->codecpar->codec_id) };
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
    setStreamHW();
    pSwsContext = sws_getContext(
        pAVCodecContext->width, pAVCodecContext->height, pAVCodecContext->pix_fmt,
        videoStreamWidth, videoStreamHeight, AV_PIX_FMT_BGR24, SWS_BILINEAR, NULL, NULL, NULL);
    pAVPacket = av_packet_alloc();
    pAVFrame = av_frame_alloc();
    return true;
}

void QRCodeForStream::continueLastLogin()
{
    switch (servertype)
    {
        using enum ServerType;
    case Official:
        ret = o.scanConfirm();
        break;
    case BH3_BiliBili:
        ret = m.scanConfirm();
        break;
    default:
        break;
    }
    emit loginResults(ret);
}

void QRCodeForStream::run()
{
    threadPool.setMaxThreadCount(threadNumber);
    m_stop.store(true);
    ret = ScanRet::Type::UNKNOW;
    //TODO 获取直播流地址放在这里
    if (init())
    {
#ifndef SHOW
        cv::namedWindow("Video_Stream", cv::WINDOW_NORMAL);
        cv::resizeWindow("Video_Stream", videoStreamWidth / 2, videoStreamHeight / 2);
#endif
        switch (servertype)
        {
            using enum ServerType;
        case Official:
            LoginOfficial();
            break;
        case BH3_BiliBili:
            LoginBH3BiliBili();
            break;
        default:
            break;
        }
    }
    if (ret == ScanRet::Type::LIVESTOP)
    {
        emit loginResults(ret);
    }
#ifndef SHOW
    cv::destroyWindow("Video_Stream");
#endif
    avformat_close_input(&pAVFormatContext);
    avcodec_free_context(&pAVCodecContext);
    sws_freeContext(pSwsContext);
    av_dict_free(&pAvdictionary);
    av_frame_free(&pAVFrame);
    av_packet_free(&pAVPacket);
    pAVFormatContext = nullptr;
    pAVCodecContext = nullptr;
    pSwsContext = nullptr;
    pAvdictionary = nullptr;
    pAVFrame = nullptr;
    pAVPacket = nullptr;
}

#if 0
void QRCodeForStream::LoginOfficial()
{
	OfficialApi o;
	std::string uuid = o.generateUUID();
	do
	{

	} while (false);
	ThreadSacn threadsacn;
	VideoProcessor vp;
	QThread::msleep(5000);
	//错误判断
	if (vp.OpenVideo("./Cache/output.flv") != 0)
	{
	}
	int64_t latestTimestamp = av_gettime_relative();
	//if (vp.avformatContext->streams[vp.index]->start_time != AV_NOPTS_VALUE)
	//{
	//	int64_t streamTimestamp = av_rescale_q(vp.avformatContext->streams[vp.index]->start_time,
	//		vp.avformatContext->streams[vp.index]->time_base, { 1, AV_TIME_BASE });
	//	if (streamTimestamp > latestTimestamp)
	//	{
	//		latestTimestamp = streamTimestamp;
	//	}
	//}
	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	int f = 0;
#ifdef _DEBUG
	cv::namedWindow("Video");
#endif // _DEBUG
	while (true)
	{
		//int64_t video_duration_sec = vp.avformatContext->duration / AV_TIME_BASE;
		//size_t timeInSeconds = av_q2d(vp.avstream->time_base);
		if (stopStream)
		{
#ifdef _DEBUG
			cv::destroyWindow("Video");
#endif // _DEBUG
			break;
		}
		int op1 = vp.read();
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		if (vp.SendPacket() != 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			QThread::msleep(300);
		}
		vp.ReceiveFrame();
		//转换像素格式
		sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
			vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
		cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
		//NOTE:可能有1280 1980和60帧 30帧,二维码不一定在屏幕的正中央。
		//cv::Mat crop_img = img(cv::Range(300, 800), cv::Range(500, 1186));
		cv::Mat _img = img(cv::Rect(0, 0, 1280, 720));

		//cv::Mat _img(720, 1280, CV_8UC3);
		//uint8_t* dstData[1] = { _img.data };
		//int dstLinesize[1] = { _img.step };
		//int code = sws_scale(vp.swsCtx, vp.pFrameBGR->data, vp.pFrameBGR->linesize, 0, vp.avCodecContext->height, dstData, dstLinesize);

		if (!threadsacn.isRunning())
		{
			threadsacn.setImg(_img);
			threadsacn.start();
			//#ifdef _DEBUG
			std::cout << "scan count " << f++ << std::endl;
			cv::imshow("Video", _img);
			//#endif
			cv::waitKey(2);
		}
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Honkai3);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			continue;
		}
		//TODO:Genshin
		if (threadsacn.getQRcode().find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::StarRail);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			continue;
		}
		av_packet_unref(vp.avPacket);
	}
}

void QRCodeForStream::LoginBH3BiliBili()
{
	do
	{

	} while (false);
	Mihoyosdk m;
	const std::string LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);
	ThreadSacn threadsacn;
	QThread::msleep(5000);
	VideoProcessor vp;
	//错误判断
	if (vp.OpenVideo("./Cache/output.flv") != 0)
	{
		throw std::logic_error("parse number error");
	}
	int64_t latestTimestamp = av_gettime_relative();
	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	int f = 0;
#ifdef _DEBUG
	cv::namedWindow("Video");
#endif // _DEBUG
	while (true)
	{
		if (m_stop)
		{
#ifdef _DEBUG
			cv::destroyWindow("Video");
#endif // _DEBUG
			break;
		}
		int op1 = vp.read();
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		if (vp.SendPacket() != 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			QThread::msleep(300);
		}
		vp.ReceiveFrame();
		// 转换像素格式
		sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
			vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
		cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
		cv::Mat _img = img(cv::Rect(0, 0, 1280, 720));
		if (!threadsacn.isRunning())
		{
			threadsacn.setImg(_img);
			threadsacn.start();
			//#ifdef _DEBUG
			std::cout << "scan count " << f++ << std::endl;
			imshow("Video", _img);
			//#endif
			cv::waitKey(2);
		}
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			continue;
		}
		av_packet_unref(vp.avPacket);
	}
}
#endif