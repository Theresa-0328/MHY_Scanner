#include "ThreadSacn.h"

#include <chrono>

ThreadSacn::ThreadSacn()
	: QThread()
	, QRScanner()
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

std::string ThreadSacn::getTicket()const
{
	return m_qrcode.substr(m_qrcode.length() - 24);
}

std::string ThreadSacn::getQRcode() const
{
	return m_qrcode;
}

void ThreadSacn::run()
{
#ifdef _DEBUG
	auto startTime = std::chrono::high_resolution_clock::now();
#endif // _DEBUG
	decodeSingle(img, m_qrcode);
#ifdef _DEBUG
	auto endTime = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
	std::cout << (double)duration / 1000000 << std::endl;
#endif // _DEBUG
}