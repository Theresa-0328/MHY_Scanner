#include "QRCodeForScreen.h"

#include <chrono>
#include <thread>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThreadPool>
#include <json.h>

#include "ScreenScan.h"
#include "QRScanner.h"


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
	ScreenScan screenshot;
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(threadNumber);
	GameType::Type m_gametype = GameType::Type::UNKNOW;

	cv::namedWindow("Video_Stream", cv::WINDOW_NORMAL);
	cv::resizeWindow("Video_Stream", 1280, 720);

	while (m_stop)
	{
		cv::Mat img;
		cv::resize(screenshot.getScreenshot(), img, { 1280,720 });

		cv::imshow("Video_Stream", img);
		cv::waitKey(1);

		threadPool.tryStart([&, temp_img = std::move(img)]()
			{
				thread_local QRScanner qrScanners;
				std::string str;
				qrScanners.decodeSingle(temp_img, str);
				if (str.size() < 85)
				{
					return;
				}
				else if (str.compare(79, 3, "8F3") == 0)
				{
					m_gametype = GameType::Type::Honkai3;
				}
				else if (str.compare(79, 3, "9E&") == 0)
				{
					m_gametype = GameType::Type::Genshin;
				}
				else if (str.compare(79, 3, "8F%") == 0)
				{
					m_gametype = GameType::Type::StarRail;
				}
				else
				{
					return;
				}
				const std::string& ticket = str.substr(str.length() - 24);
				if (!o.scanInit(m_gametype, ticket, uid, gameToken))
				{
					return;
				}
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
						emit loginConfirm(m_gametype, true);
					}
				}
				else
				{
					emit loginResults(ret);
				}
				stop();
			}
		);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
}

void QRCodeForScreen::LoginBH3BiliBili()
{
	ScreenScan screenshot;
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(threadNumber);
	const std::string& LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);

	while (m_stop)
	{
		cv::Mat img;
		cv::resize(screenshot.getScreenshot(), img, { 1280,720 });

		cv::imshow("Video_Stream", img);
		cv::waitKey(1);

		threadPool.tryStart([&, temp_img = std::move(img)]()
			{
				thread_local QRScanner qrScanners;
				std::string str;
				qrScanners.decodeSingle(temp_img, str);
				if (str.size() < 85)
				{
					return;
				}
				else if (str.compare(79, 3, "8F3") != 0)
				{
					return;
				}
				const std::string& ticket = str.substr(str.length() - 24);
				if (!m.scanInit(ticket, LoginData))
				{
					return;
				}
				if (ret = m.scanCheck(); ret == ScanRet::Type::SUCCESS)
				{
					json::Json config;
					config.parse(m_config->getConfig());
					if (config["auto_login"])
					{
						ret = m.scanConfirm();
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
			}
		);
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
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
