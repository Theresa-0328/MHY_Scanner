#pragma once

#include <functional>

#include <QMutex>
#include <QThread>

#include "Common.h"
#include "ConfigDate.h"
#include "Mihoyosdk.h"
#include "ScannerBase.hpp"

class QRCodeForScreen final :
    public QThread,
    public ScannerBase
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
    Mihoyosdk m;
    ConfigDate* m_config;
    void LoginOfficial();
    void LoginBH3BiliBili();
    std::atomic<bool> m_stop;
    std::string m_name;
    GameType m_gametype{ GameType::UNKNOW };
    ServerType servertype{};
    ScanRet ret{ ScanRet::UNKNOW };
    const int threadNumber{ 1 };
};