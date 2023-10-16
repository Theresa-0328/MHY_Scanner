#pragma once

#include <initializer_list>
#include <functional>

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
	void continueLastLogin();
	void run();
	void stop();
signals:
	void loginResults(const ScanRet::Type ret);
	void loginConfirm(const GameType::Type gameType, bool b);
private:
	OfficialApi o;
	Mihoyosdk m;
	ConfigDate* m_config;
	void LoginOfficial();
	void LoginBH3BiliBili();
	bool m_stop = false;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string m_name;
	const std::map<std::string_view, std::function<void()>> setGameType
	{
		{"8F3", [this]() { m_gametype = GameType::Type::Honkai3; }},
		{"9E&", [this]() { m_gametype = GameType::Type::Genshin; }},
		{"8F%", [this]() { m_gametype = GameType::Type::StarRail; }},
	};
	GameType::Type m_gametype = GameType::Type::UNKNOW;
	ServerType::Type servertype = ServerType::Type::UNKNOW;
	ScanRet::Type ret = ScanRet::Type::UNKNOW;
	const int threadNumber = 1;
};