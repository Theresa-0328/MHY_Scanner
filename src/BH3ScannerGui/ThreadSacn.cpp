#include "ThreadSacn.h"

ThreadSacn::ThreadSacn(QObject *parent)
	: QThread(parent)
{

}

ThreadSacn::~ThreadSacn()
{
	
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
