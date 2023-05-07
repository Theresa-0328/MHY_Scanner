#include "ThreadSacn.h"
#include "QRScanner.h"

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
	QRScanner qs;
	qs.Decode(tImg, uqrcode);

}
