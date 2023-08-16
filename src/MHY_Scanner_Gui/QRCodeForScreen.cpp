#include "QRCodeForScreen.h"

#include "OfficialApi.h"
#include "ScreenScan.h"
#include "Mihoyosdk.h"


QRCodeForScreen::QRCodeForScreen(QObject* parent)
	: QThread(parent)
{

}

QRCodeForScreen::~QRCodeForScreen()
{
	if (!this->isInterruptionRequested())
	{
		QMutexLocker lock(&m_mux);
		Exit = true;
	}
	this->requestInterruption();
	this->wait();
}

void QRCodeForScreen::setLoginInfo(const std::string& uid, const std::string& token)
{
	this->uid = uid;
	this->gameToken = token;
}

void QRCodeForScreen::setLoginInfo(const std::string& uid, const std::string& token, const std::string& name)
{
	this->uid = uid;
	this->gameToken = token;
	this->m_name = name;
}

void QRCodeForScreen::LoginOfficial()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	OfficialApi o;
	const std::string& uuid = o.generateUUID();
	threadsacn.start();
	while (!Exit)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		if (qrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Honkai3);
			const int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		if (qrcode.find("biz_key=hk4e_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::Genshin);
			const int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		if (qrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.setGameType(GameType::Type::StarRail);
			const int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
			emit loginResults(code == 0);
			break;
		}
		cv::waitKey(222);
	}
	threadsacn.stop();
	return;
}

void QRCodeForScreen::LoginBH3BiliBili()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	Mihoyosdk m;
	const std::string& LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);
	threadsacn.start();
	while (!Exit)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		if (qrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			const int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			break;
		}
		cv::waitKey(222);
	}
	threadsacn.stop();
	return;
}

void QRCodeForScreen::run()
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

void QRCodeForScreen::stop()
{
	Exit = true;
}

void QRCodeForScreen::setServerType(const ServerType::Type& servertype)
{
	this->servertype = servertype;
}
