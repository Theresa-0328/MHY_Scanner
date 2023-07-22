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
	const std::string uuid = o.generateUUID();
	while (!Exit)
	{
		img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Honkai3);
			const int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			return;
		}
		//if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		//{
		//}
		if (threadsacn.getQRcode().find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::StarRail);
			const int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			return;
		}
		cv::waitKey(222);
	}
	return;
}

void ThreadGetScreen::LoginBH3BiliBili()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	cv::Mat img;
	Mihoyosdk m;
	const std::string LoginData = m.verify(uid, gameToken);
	m.setUserName(uname);
	while (!Exit)
	{
		img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		threadsacn.start();
		if (threadsacn.getQRcode().find("biz_key=bh3_cn") != std::string::npos)
		{
			const int code = m.scanCheck(threadsacn.getTicket(), LoginData);
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
	case ServerType::BH3_BiliBili:
		LoginBH3BiliBili();
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
