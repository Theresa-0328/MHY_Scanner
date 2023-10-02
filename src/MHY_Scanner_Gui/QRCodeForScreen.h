#pragma once

#include <initializer_list>

#include <QThread>
#include <QMutex>

#include "Common.h"
#include "Mihoyosdk.h"
#include "OfficialApi.h"
#include "ConfigDate.h"

class QRCodeForScreen
	: public QThread
{
	Q_OBJECT
public:
	QRCodeForScreen(QObject* parent);
	~QRCodeForScreen();
	void setLoginInfo(const std::string& uid, const std::string& token);
	void setLoginInfo(const std::string& uid, const std::string& token, const std::string& name);
	void setServerType(const ServerType::Type& servertype);
	void run();
	void stop();
	bool continueLogin = false;
signals:
	void loginResults(const ScanRet::Type ret);
	void loginConfirm(const GameType::Type gameType);
private:
	OfficialApi o;
	Mihoyosdk m;
	ConfigDate* m_config;
	void LoginOfficial();
	void LoginBH3BiliBili();
	void continueLastLogin();
	bool m_stop = false;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string m_name;
	ServerType::Type servertype = ServerType::Type::UNKNOW;
	ScanRet::Type ret = ScanRet::Type::UNKNOW;
};