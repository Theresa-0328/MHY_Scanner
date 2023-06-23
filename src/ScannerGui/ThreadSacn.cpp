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

std::string ThreadSacn::getTicket()
{
	size_t pos = uqrcode.find("ticket=");
	if (pos != std::string::npos)
	{
		return uqrcode.substr(pos + 7);
	}
	return "";
}

void ThreadSacn::run()
{
	qs.Decode(img, uqrcode);
	QThread::msleep(100);
}