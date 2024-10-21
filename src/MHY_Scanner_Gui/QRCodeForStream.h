#pragma once

#include <atomic>

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
#include <Json.h>

#include "OfficialApi.h"
#include "Mihoyosdk.h"
#include "Common.h"
#include "ConfigDate.h"

class QRCodeForStream final :
    public QThread
{
    Q_OBJECT
public:
    QRCodeForStream(QObject* parent = nullptr);
    ~QRCodeForStream();
    void setLoginInfo(const std::string& uid, const std::string& gameToken);
    void setLoginInfo(const std::string& uid, const std::string& gameToken, const std::string& name);
    void setServerType(const ServerType servertype);
    void setUrl(const std::string& url, const std::map<std::string, std::string> heard = {});
    auto init() -> bool;
    void run();
    void stop();
    void continueLastLogin();
signals:
    void loginResults(const ScanRet ret);
    void loginConfirm(const GameType gameType, bool b);

private:
    OfficialApi o;
    Mihoyosdk m;
    std::string m_url;
    void LoginOfficial();
    void LoginBH3BiliBili();
    void setStreamHW();
    std::string uid;
    std::string gameToken;
    std::string m_name;
    ConfigDate* m_config;
    std::map<std::string_view, std::function<void()>> setGameType{
        { "8F3", [this]() { m_gametype = GameType::Honkai3; } },
        { "9E&", [this]() { m_gametype = GameType::Genshin; } },
        { "8F%", [this]() { m_gametype = GameType::HonkaiStarRail; } },
        { "%BA", [this]() { m_gametype = GameType::ZenlessZoneZero; } },
    };
    GameType m_gametype = GameType::UNKNOW;
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
    const int threadNumber{ 2 };
    QThreadPool threadPool;
    std::atomic<bool> m_stop;
};