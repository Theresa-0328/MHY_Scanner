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


void ThreadGetScreen::Init0(const std::string& uid, const std::string& token)
{
	this->uid = uid;
	this->gameToken = token;
}

void ThreadGetScreen::Init1(const std::string& uid, const std::string& token, const std::string& uname)
{
	this->uid = uid;
	this->gameToken = token;
	this->uname = uname;
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
		cv::waitKey(222);
	}
	return;
}

void ThreadGetScreen::serverType1()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	cv::Mat img;
	std::string LoginData;
	Mihoyosdk m;
	LoginData = m.verify(std::stoi(uid), gameToken);
	m.setUserName(uname);
	while (!isExit)
	{
		img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			return;
		}
		cv::waitKey(200);
	}
}

void ThreadGetScreen::run()
{
	isExit = false;
	if (serverType == 0)
	{
		serverType0();
		return;
	}
	if (serverType == 1)
	{
		serverType1();
		return;
	}
	return;
}

void ThreadGetScreen::stop()
{
	isExit = true;
}
