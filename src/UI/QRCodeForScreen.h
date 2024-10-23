#pragma once

#include <functional>

#include <QMutex>
#include <QThread>

#include "Common.h"
#include "ConfigDate.h"
#include "Mihoyosdk.h"
#include "OfficialApi.h"

class QRCodeForScreen final :
    public QThread
{
    Q_OBJECT
public:
    QRCodeForScreen(QObject* parent);
    ~QRCodeForScreen();
    Q_DISABLE_COPY_MOVE(QRCodeForScreen)

    void setLoginInfo(const std::string& uid, const std::string& token);
    void setLoginInfo(const std::string& uid, const std::string& token, const std::string& name);
    void setServerType(const ServerType servertype);
    void continueLastLogin();
    void run();
    void stop();
signals:
    void loginResults(const ScanRet ret);
    void loginConfirm(const GameType gameType, bool b);

private:
    OfficialApi o;
    Mihoyosdk m;
    ConfigDate* m_config;
    void LoginOfficial();
    void LoginBH3BiliBili();
    std::atomic<bool> m_stop;
    std::string uid;
    std::string gameToken;
    std::string m_name;
    std::map<std::string_view, std::function<void()>> setGameType{
        { "8F3", [this]() { m_gametype = GameType::Honkai3; } },
        { "9E&", [this]() { m_gametype = GameType::Genshin; } },
        { "8F%", [this]() { m_gametype = GameType::HonkaiStarRail; } },
        { "%BA", [this]() { m_gametype = GameType::ZenlessZoneZero; } },
    };
    GameType m_gametype{ GameType::UNKNOW };
    ServerType servertype{};
    ScanRet ret{ ScanRet::UNKNOW };
    const int threadNumber{ 1 };
};