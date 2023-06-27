#pragma once
#include <QThread>
#include <QMutex>
#include "Mihoyosdk.h"
#include "ScreenScan.h"
#include "QRScanner.h"
#include "VideoProcessor.h"
class ThreadStreamProcess  : public QThread
{
	Q_OBJECT
public:
	ThreadStreamProcess(QObject *parent = nullptr);
	~ThreadStreamProcess();
	void Init0(const std::string uid, const std::string gameToken);
	void Init1(const std::string uid, const std::string gameToken, const std::string uName);
	void stop();
	void run();
	std::string url;
	int serverType;
signals:
	void loginResults(const bool& b);
private:
	void serverType0();
	void serverType1();
	std::string LoginData;
	bool stopStream =false;
	Mihoyosdk m;
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string uName;
};