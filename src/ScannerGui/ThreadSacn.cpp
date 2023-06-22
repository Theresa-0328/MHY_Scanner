#include "ThreadSacn.h"

ThreadSacn::ThreadSacn()
	: QThread()
{

}

ThreadSacn::~ThreadSacn()
{
	this->requestInterruption();
	this->wait();
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