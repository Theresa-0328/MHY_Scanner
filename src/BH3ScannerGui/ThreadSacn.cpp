#include "ThreadSacn.h"

ThreadSacn::ThreadSacn()
	: QThread()
{

}

ThreadSacn::~ThreadSacn()
{
	while (isRunning())
	{
		QThread::msleep(150);
	}
}

void ThreadSacn::setImg(cv::Mat img)
{
	this->img = img;
}


void ThreadSacn::run()
{
	qs.Decode(img, uqrcode);
	QThread::msleep(100);
}