#include "QRCodeForStream.h"

#include <string>
#include <string_view>

#include "QRScanner.h"
#include "MhyApi.hpp"

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

void QRCodeForStream::setLoginInfo(const std::string_view uid, const std::string_view gameToken)
{
    this->uid = uid;
    this->gameToken = gameToken;
}

void QRCodeForStream::setLoginInfo(const std::string_view uid, const std::string_view gameToken, const std::string& name)
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
    while (m_stop.load())
    {
        if (av_read_frame(pAVFormatContext, pAVPacket) < 0)
        {
            ret = ScanRet::LIVESTOP;
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
            ret = ScanRet::LIVESTOP;
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
                if (!setGameType.contains(view))
                {
                    return;
                }
                const std::string_view ticket(str.data() + str.size() - 24, 24);
                setGameType[view]();
                if (lastTicket == ticket)
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
                    if (ScanQRLogin(scanUrl.data(), ticket, gameType))
                    {
                        lastTicket = ticket;
                        json::Json config;
                        config.parse(m_config->getConfig());
                        if (config["auto_login"])
                        {
                            continueLastLogin();
                        }
                        else
                        {
                            Q_EMIT loginConfirm(gameType, false);
                        }
                    }
                    else
                    {
                        Q_EMIT loginResults(ScanRet::FAILURE_1);
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
    while (m_stop.load())
    {
        if (av_read_frame(pAVFormatContext, pAVPacket) < 0)
        {
            ret = ScanRet::LIVESTOP;
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
            ret = ScanRet::LIVESTOP;
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
                    if (ret = m.scanCheck(); ret == ScanRet::SUCCESS)
                    {
                        json::Json config;
                        config.parse(m_config->getConfig());
                        if (config["auto_login"])
                        {
                            ret = m.scanConfirm();
                            Q_EMIT loginResults(ret);
                        }
                        else
                        {
                            Q_EMIT loginConfirm(GameType::Honkai3_BiliBili, false);
                        }
                    }
                    else
                    {
                        Q_EMIT loginResults(ret);
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
    streamUrl = url;
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
    if (avformat_open_input(&pAVFormatContext, streamUrl.c_str(), NULL, &pAvdictionary) != 0)
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
    {
        bool b = ConfirmQRLogin(confirmUrl, uid, gameToken, lastTicket, gameType);
        if (b)
        {
            Q_EMIT loginResults(ScanRet::SUCCESS);
        }
        else
        {
            Q_EMIT loginResults(ScanRet::FAILURE_2);
        }
    }
    break;
    case BH3_BiliBili:
    {
        ret = m.scanConfirm();
        Q_EMIT loginResults(ret);
    }
    break;
    default:
        break;
    }
}

void QRCodeForStream::run()
{
    threadPool.setMaxThreadCount(threadNumber);
    m_stop.store(true);
    ret = ScanRet::UNKNOW;
    //TODO 获取直播流地址放在这里
    if (init())
    {
#ifndef SHOW
        cv::namedWindow("Video_Stream", cv::WINDOW_AUTOSIZE);
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
    if (ret == ScanRet::LIVESTOP)
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