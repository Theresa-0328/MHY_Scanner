#include "ThreadGetScreen.h"
#include "ThreadSacn.h"

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
	ThreadSacn ts1;
	isExit = false;
	while (!isExit)
	{
		img = screenshot.getScreenshot();
		//img = screenshot.getScreenshot(600,250,600,600);
		ts1.setImg(img);
		ts1.run();
		if (ts1.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(ts1.uqrcode, LoginData);
			emit loginResults(code == 0);
			break;
		}
		cv::waitKey(200);
	}
}