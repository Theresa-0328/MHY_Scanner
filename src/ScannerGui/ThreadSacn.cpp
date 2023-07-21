#include "ThreadSacn.h"
#include <chrono>

ThreadSacn::ThreadSacn()
	: QThread()
{

}

ThreadSacn::~ThreadSacn()
{
	this->requestInterruption();
	this->wait();
}

void ThreadSacn::setImg(const cv::Mat& img)
{
	this->img = img;
}

std::string ThreadSacn::getTicket()
{
	const size_t length = uqrcode.length();
	return uqrcode.substr(length - 24);
}

void ThreadSacn::run()
{
#ifdef _DEBUG
	auto startTime = std::chrono::high_resolution_clock::now();
#endif // _DEBUG
	qs.decodeSingle(img, uqrcode);
#ifdef _DEBUG
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
	std::cout << (double)duration / 1000000 << std::endl;
#endif // _DEBUG
}