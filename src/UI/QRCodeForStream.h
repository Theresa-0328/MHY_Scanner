#pragma once

#include <atomic>
#include <chrono>
#include <memory>
#include <string_view>

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
};

#include <QThread>
#include <QMutex>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThreadPool>

#include "ApiDefs.hpp"
#include "ConfigDate.h"
#include "ScannerBase.hpp"

// OpenCV 前向声明：头文件仅以 shared_ptr 持有 cv::Mat，避免引入整个 opencv 头文件。
namespace cv { class Mat; }

class QRCodeForStream final :
    public QThread,
    public ScannerBase
{
    Q_OBJECT
public:
    QRCodeForStream(QObject* parent = nullptr);
    ~QRCodeForStream();
    Q_DISABLE_COPY_MOVE(QRCodeForStream)

    void setLoginInfo(const std::string_view uid, const std::string_view stoken);
    void setLoginInfo(const std::string_view uid, const std::string_view stoken, const std::string& name);
    void setMid(const std::string& mid);
    void setServerType(const ServerType servertype);
    void setUrl(const std::string& url, const std::map<std::string, std::string> heard = {});
    auto init() -> bool;
    void run();
    void stop();
    void continueLastLogin();

Q_SIGNALS:
    void loginResults(const ScanRet ret);
    void loginConfirm(const GameType gameType, bool b);

private:
    std::mutex mtx;
    void LoginOfficial();
    void LoginBH3BiliBili();
    void setStreamHW();
    std::string streamUrl{};
    std::string m_name;
    ConfigDate* m_config;
    ServerType servertype;
    ScanRet ret = ScanRet::UNKNOW;
    AVDictionary* pAvdictionary;
    AVFormatContext* pAVFormatContext;
    AVCodecContext* pAVCodecContext;
    SwsContext* pSwsContext;
    AVFrame* pAVFrame;
    AVPacket* pAVPacket;
    int videoStreamIndex{ 0 };
    int videoStreamWidth{};
    int videoStreamHeight{};
    const int threadNumber{ 3 };   // 解码线程池并发数（原 2，提升吞吐并降低丢帧率）
    QThreadPool threadPool;         // QR 解码线程池
    // ── 直播流可靠性增强（根治「逐帧 tryStart 丢帧 → 大概率无反应」）──
    std::shared_ptr<cv::Mat> latestFrame{ nullptr };          // 最新帧缓存，绝不丢弃
    std::chrono::steady_clock::time_point lastSubmitTime{};  // 上次提交解码的时刻（节奏限流）
    std::chrono::steady_clock::time_point lastFrameTime{};   // 上次读到帧的时刻（看门狗）
    std::atomic<bool> m_stop;
};