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
		m_stop = false;
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

	auto processQRCodeStr = [&](const std::string& qrodeStr, const std::string& bizKey, GameType::Type gameType)
		{
			if (qrodeStr.find(bizKey) != std::string::npos)
			{
				o.setGameType(gameType);
				ret = o.scanRequest(threadsacn.getTicket(), uid, gameToken, uuid);
				stop();
			}
		};

	while (m_stop)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		processQRCodeStr(qrcode, "bh3_cn", GameType::Type::Honkai3);
		processQRCodeStr(qrcode, "hk4e_cn", GameType::Type::Genshin);
		processQRCodeStr(qrcode, "hkrpg_cn", GameType::Type::StarRail);
		QThread::msleep(200);
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

	auto processQRCodeStr = [&](const std::string& qrcodeStr, const std::string& bizKey, const std::string& login_data)
		{
			if (qrcodeStr.find(bizKey) != std::string::npos)
			{
				ret = m.scanCheck(threadsacn.getTicket(), login_data);
				stop();
			}
		};

	while (m_stop)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		processQRCodeStr(qrcode, "bh3_cn", LoginData);
		QThread::msleep(200);
	}
	threadsacn.stop();
	return;
}

void QRCodeForScreen::run()
{
	ret = ScanRet::Type::UNKNOW;
	m_stop = true;
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
	emit loginResults(ret);
}

void QRCodeForScreen::stop()
{
	m_stop = false;
}

void QRCodeForScreen::setServerType(const ServerType::Type& servertype)
{
	this->servertype = servertype;
}
