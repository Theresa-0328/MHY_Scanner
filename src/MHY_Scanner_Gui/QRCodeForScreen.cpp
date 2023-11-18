#include "QRCodeForScreen.h"

#include <chrono>
#include <thread>

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThreadPool>
#include <json.h>

#include "ScreenScan.h"
#include "QRScanner.h"
#include "ScreenShotDXGI.hpp"

#define DELAYED 200

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
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(threadNumber);
	ScreenShotDXGI screenshotdxgi;
	int w{ 0 };
	int h{ 0 };
	screenshotdxgi.InitDevice();
	screenshotdxgi.InitDupl(0, w, h);
	long mBufferSize = w * h * 4;
	uint8_t* mBuffer = new UCHAR[mBufferSize];
	while (m_stop)
	{
		screenshotdxgi.getFrame(100);
		screenshotdxgi.copyFrameToBuffer(&mBuffer, mBufferSize);
		cv::Mat img;
		cv::resize(cv::Mat(h, w, CV_8UC4, mBuffer), img, { 1280,720 });
#ifndef SHOW
		cv::imshow("Video_Stream", img);
		cv::waitKey(1);
#endif
		threadPool.tryStart([&, temp_img = std::move(img)]()
			{
				thread_local QRScanner qrScanners;
				std::string str;
				qrScanners.decodeSingle(temp_img, str);
				if (str.size() < 85)
				{
					return;
				}
				std::string_view view(str.c_str() + 79, 3);
				if (setGameType.count(view) == 0)
				{
					return;
				}
				setGameType[view]();
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
		std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
		screenshotdxgi.doneWithFrame();
	}
	delete[] mBuffer;
}

void QRCodeForScreen::LoginBH3BiliBili()
{
	QThreadPool threadPool;
	threadPool.setMaxThreadCount(threadNumber);
	const std::string& LoginData = m.verify(uid, gameToken);
	m.setUserName(m_name);
	ScreenShotDXGI screenshotdxgi;
	int w{ 0 };
	int h{ 0 };
	screenshotdxgi.InitDevice();
	screenshotdxgi.InitDupl(0, w, h);
	long mBufferSize = w * h * 4;
	uint8_t* mBuffer = new UCHAR[mBufferSize];
	while (m_stop)
	{
		screenshotdxgi.getFrame(100);
		screenshotdxgi.copyFrameToBuffer(&mBuffer, mBufferSize);
		cv::Mat img;
		cv::resize(cv::Mat(h, w, CV_8UC4, mBuffer), img, { 1280,720 });
#ifndef SHOW
		cv::imshow("Video_Stream", img);
		cv::waitKey(1);
#endif
		threadPool.tryStart([&, temp_img = std::move(img)]()
			{
				thread_local QRScanner qrScanners;
				std::string str;
				qrScanners.decodeSingle(temp_img, str);
				if (str.size() < 85)
				{
					return;
				}
				if (std::string_view view(str.c_str() + 79, 3); view != "8F3")
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
		std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
		screenshotdxgi.doneWithFrame();
	}
	delete[] mBuffer;
}

void QRCodeForScreen::continueLastLogin()
{
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
	ret = ScanRet::Type::UNKNOW;
	m_stop = true;
#ifndef SHOW
	cv::namedWindow("Video_Stream", cv::WINDOW_AUTOSIZE);
#endif
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
#ifndef SHOW
	cv::destroyWindow("Video_Stream");
#endif
}

void QRCodeForScreen::stop()
{
	m_stop = false;
}

void QRCodeForScreen::setServerType(const ServerType::Type& servertype)
{
	this->servertype = servertype;
}
