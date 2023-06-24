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

void ThreadGetScreen::InitScreen(int uid, std::string access_key, std::string uname)
{
	LoginData = m.verify(uid, access_key);
	m.setUserName(uname);
}

void ThreadGetScreen::run()
{
	ScreenScan screenshot;
	cv::Mat img;
	ThreadSacn threadsacn;
	isExit = false;
	while (!isExit)
	{
		img = screenshot.getScreenshot();
		//img = screenshot.getScreenshot(600,250,600,600);
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			break;
		}
		if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		{
			OfficialApi o;
			o.gameType = 8;
			std::string text = "";
			o.cookieParser(text);
			o.ticket = threadsacn.getTicket();
			o.scanRequest();
		}
		cv::waitKey(250);
	}
}

void ThreadGetScreen::stop()
{
	isExit = true;
}
