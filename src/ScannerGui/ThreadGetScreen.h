#pragma once
#include <QThread>
#include <QMutex>
#include "ScreenScan.h"
#include "Mihoyosdk.h"

class ServerType
{
public:
	enum Type
	{
		Official = 0,
		BiliBili
	};
};

class ThreadGetScreen
	: public QThread
{

	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	void SetLoginInfo(const std::string uid, const std::string token);
	void SetLoginInfo(const std::string uid, const std::string token, const std::string uname);
	void run();
	void stop();
	void setServerType(ServerType::Type servertype);
signals:
	void loginResults(const bool& b);
private:
	void LoginOfficial();
	void LoginBiliBili();
	bool Exit = true;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string uname;
	ServerType::Type servertype;
};