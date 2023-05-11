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
	tImg = img;
}


void ThreadSacn::run()
{
	qs.Decode(tImg, uqrcode);
	QThread::msleep(100);
}
