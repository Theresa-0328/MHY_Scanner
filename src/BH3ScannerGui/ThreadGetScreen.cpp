#include "ThreadGetScreen.h"
#include "Bsgsdk.h"
#include "ScreenScan.h"
#include "QRScanner.h"
//#include "Mihoyosdk.h" //临时

ThreadGetScreen::ThreadGetScreen(QObject* parent)
	: QThread(parent)
{
	openConfig();
	loginBiliBiliKey();
	loginHonkai3();
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

void ThreadGetScreen::run()
{
	ScreenScan screenshot;
	cv::Mat img;
	QRScanner s;
	isExit = false;
	bool b = true;//登录是否成功
	while (!isExit)
	{
		std::string deCode;
		img = screenshot.getScreenshot();
		//img = screenshot.getScreenshot(600,250,600,600);
		s.Decode(img, deCode);
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			scanQRCode(deCode);
			emit loginResults(b);
			break;
		}
		cv::waitKey(100);
	}
}