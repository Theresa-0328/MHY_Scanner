#include "ThreadGetScreen.h"

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
	QRScanner s;
	isExit = false;
	while (!isExit)
	{
		std::string deCode;
		img = screenshot.getScreenshot();
		//img = screenshot.getScreenshot(600,250,600,600);
		s.Decode(img, deCode);
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(deCode, LoginData);
			emit loginResults(code == 0);
			break;
		}
		cv::waitKey(200);
	}
}