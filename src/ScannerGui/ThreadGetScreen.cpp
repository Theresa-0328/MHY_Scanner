#include "ThreadGetScreen.h"
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
		Exit = true;
	}
	this->requestInterruption();
	this->wait();
}


void ThreadGetScreen::setLoginInfo(const std::string uid, const std::string token)
{
	this->uid = uid;
	this->gameToken = token;
}

void ThreadGetScreen::setLoginInfo(const std::string uid, const std::string token, const std::string uname)
{
	this->uid = uid;
	this->gameToken = token;
	this->uname = uname;
}

void ThreadGetScreen::LoginOfficial()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	cv::Mat img;
	OfficialApi o;
	while (!Exit)
	{
		img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Honkai3);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			return;
		}
		//if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		//{
		//}
		if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::StarRail);
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			return;
		}
		cv::waitKey(222);
	}
	return;
}

void ThreadGetScreen::LoginBiliBili()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	cv::Mat img;
	std::string LoginData;
	Mihoyosdk m;
	LoginData = m.verify(std::stoi(uid), gameToken);
	m.setUserName(uname);
	while (!Exit)
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
		cv::waitKey(222);
	}
}

void ThreadGetScreen::run()
{
	Exit = false;
	switch (servertype)
	{
	case ServerType::Official:
		LoginOfficial();
		break;
	case ServerType::BiliBili:
		LoginBiliBili();
		break;
	default:
		break;
	}
}

void ThreadGetScreen::stop()
{
	Exit = true;
}

void ThreadGetScreen::setServerType(ServerType::Type servertype)
{
	this->servertype = servertype;
}
