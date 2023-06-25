#include "ThreadGetScreen.h"
#include "ThreadSacn.h"
#include <OfficialApi.h>

ThreadGetScreen::ThreadGetScreen(QObject* parent)
	: QThread(parent)
{

}

ThreadGetScreen::~ThreadGetScreen()
{
	if (!this->isInterruptionRequested())
	{
		QMutexLocker lock(&m_mux);
		isExit = true;
	}
	this->requestInterruption();
	this->wait();
}

void ThreadGetScreen::InitScreen(const int& uid, const std::string& access_key, std::string uname)
{
	LoginData = m.verify(uid, access_key);
	m.setUserName(uname);
}

void ThreadGetScreen::Init0(const std::string& uid, const std::string& token)
{
	this->uid = uid;
	this->gameToken = token;
}

void ThreadGetScreen::serverType0()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	cv::Mat img;
	OfficialApi o;
	while (!isExit)
	{
		img = screenshot.getScreenshot();
		//img = screenshot.getScreenshot(600,250,600,600);
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			o.gameType = 1;
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			return;
		}
		//if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		//{
		//	o.gameType = 8;
		//	int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
		//	emit loginResults(code == 0);
		//	return;
		//}
		if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.gameType = 8;
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			return;
		}
		cv::waitKey(200);
	}
	return;
}

void ThreadGetScreen::run()
{
	ScreenScan screenshot;
	cv::Mat img;
	ThreadSacn threadsacn;
	OfficialApi o;
	int uid;
	std::string ticket;
	isExit = false;
	if (serverType == 0)
	{
		serverType0();
		return;
	}
	if (serverType == 1)
	{

	}
	return;
}

void ThreadGetScreen::stop()
{
	isExit = true;
}
