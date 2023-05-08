#include "ThreadSacn.h"

ThreadSacn::ThreadSacn(QObject *parent)
	: QThread(parent)
{

}

ThreadSacn::~ThreadSacn()
{
	
}

void ThreadSacn::getImg(cv::Mat img)
{
	tImg = img;
}


void ThreadSacn::run()
{
	qs.Decode(tImg, uqrcode);
}
