#pragma once

#include <QThread>
#include <QMutex>

#include "ThreadSacn.h"

class ThreadStreamProcess : public QThread
{
	Q_OBJECT
public:
	ThreadStreamProcess(QObject* parent = nullptr);
	~ThreadStreamProcess();
	void setLoginInfo(const std::string uid, const std::string gameToken);
	void setLoginInfo(const std::string uid, const std::string gameToken, const std::string name);
	void setServerType(const ServerType::Type& servertype);
	void stop();
	void run();
	std::string url;
signals:
	void loginResults(const bool& b);
private:
	void LoginOfficial();
	void LoginBH3BiliBili();
	bool stopStream = false;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string m_name;
	ServerType::Type servertype = ServerType::Type::UNKNOW;
};