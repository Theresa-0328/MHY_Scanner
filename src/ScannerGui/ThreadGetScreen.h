#pragma once
#include <QThread>
#include <QMutex>
#include "ScreenScan.h"
#include "Mihoyosdk.h"
#include "ThreadSacn.h"

class ThreadGetScreen
	: public QThread
{

	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	void setLoginInfo(const std::string uid, const std::string token);
	void setLoginInfo(const std::string uid, const std::string token, const std::string uname);
	void setServerType(ServerType::Type servertype);
	void run();
	void stop();
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