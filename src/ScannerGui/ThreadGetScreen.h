#pragma once
#include <QThread>
#include <QMutex>
#include "ScreenScan.h"
#include "Mihoyosdk.h"
class ThreadGetScreen
	: public QThread
{

	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	int serverType = 0;
	void Init0(const std::string& uid, const std::string& token);
	void Init1(const std::string& uid, const std::string& token, const std::string& uname);
	void run();
	void stop();
signals:
	void loginResults(const bool& b);
private:
	void serverType0();
	void serverType1();
	bool isExit = true;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string uname;
};