#include "QRCodeForScreen.h"

#include <chrono>
#include <thread>

#include <json.h>

#include "ScreenScan.h"
#include "ThreadSacn.h"


QRCodeForScreen::QRCodeForScreen(QObject* parent)
	: QThread(parent)
{
	m_config = &ConfigDate::getInstance();
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
	threadsacn.start();

	auto processQRCodeStr = [&](const std::string& qrcodeStr, const std::string& bizKey, GameType::Type gameType)
		{
			if (qrcodeStr != bizKey)
			{
				return;
			}
			o.scanInit(gameType, threadsacn.getTicket(), uid, gameToken);
			if (ret = o.scanRequest(); ret == ScanRet::Type::SUCCESS)
			{
				json::Json config;
				config.parse(m_config->getConfig());
				if (config["auto_login"])
				{
					ret = o.scanConfirm();
					emit loginResults(ret);
				}
				else
				{
					emit loginConfirm(gameType, true);
				}
			}
			else
			{
				emit loginResults(ret);
			}
			stop();

		};

	while (m_stop)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		if (size_t found = qrcode.find("biz_key="); found != std::string::npos)
		{
			const std::string& key = qrcode.substr(found + 8, 3);
			processQRCodeStr(key, "bh3", GameType::Type::Honkai3);
			processQRCodeStr(key, "hk4", GameType::Type::Genshin);
			processQRCodeStr(key, "hkr", GameType::Type::StarRail);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	threadsacn.stop();
	return;
}

void QRCodeForScreen::LoginBH3BiliBili()
{
	ThreadSacn threadsacn;
	ScreenScan screenshot;
	const std::string& LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);
	threadsacn.start();

	auto processQRCodeStr = [&](const std::string& qrcodeStr, const std::string& bizKey)
		{
			if (qrcodeStr.find(bizKey) == std::string::npos)
			{
				return;
			}
			m.scanInit(threadsacn.getTicket(), LoginData);
			if (ret = m.scanCheck(); ret == ScanRet::Type::SUCCESS)
			{
				json::Json config;
				config.parse(m_config->getConfig());
				if (config["auto_login"])
				{
					m.scanConfirm();
					emit loginResults(ret);
				}
				else
				{
					emit loginConfirm(GameType::Type::Honkai3_BiliBili, true);
				}

			}
			else
			{
				emit loginResults(ret);
			}
			stop();
		};

	while (m_stop)
	{
		const cv::Mat& img = screenshot.getScreenshot();
		threadsacn.setImg(img);
		const std::string& qrcode = threadsacn.getQRcode();
		processQRCodeStr(qrcode, "biz_key=bh3_cn");
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	threadsacn.stop();
	return;
}

void QRCodeForScreen::continueLastLogin()
{
	continueLogin = false;
	switch (servertype)
	{
	case ServerType::Official:
		ret = o.scanConfirm();
		break;
	case ServerType::BH3_BiliBili:
		ret = m.scanConfirm();
		break;
	default:
		break;
	}
	emit loginResults(ret);
}

void QRCodeForScreen::run()
{
	if (continueLogin)
	{
		continueLastLogin();
		return;
	}
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
}

void QRCodeForScreen::stop()
{
	m_stop = false;
}

void QRCodeForScreen::setServerType(const ServerType::Type& servertype)
{
	this->servertype = servertype;
}
