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
	stop();
	this->wait();
}

void ThreadSacn::setImg(const cv::Mat& img)
{
	this->m_img = img;
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
#ifndef SHOW
	cv::namedWindow("Video_Stream", cv::WINDOW_NORMAL);
	cv::resizeWindow("Video_Stream", 1280, 720);
#endif
	m_stop = true;
	while (m_stop)
	{
		if (!MatEmpty())
		{
#ifndef TESTSPEED
			auto startTime = std::chrono::high_resolution_clock::now();
#endif
			decodeMultiple(m_img, m_qrcode);
#ifndef SHOW
			cv::imshow("Video_Stream", m_img);
			cv::waitKey(1);
#endif
			m_img.release();
#ifndef TESTSPEED
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
			std::cout << static_cast<float>(duration) / 1000000 << std::endl;
#endif
		}
		else
		{
			cv::waitKey(10);
		}
	}
#ifndef SHOW
	cv::destroyWindow("Video_Stream");
#endif
}

void ThreadSacn::stop()
{
	QMutexLocker lock(&m_mux);
	m_stop = false;
}

bool ThreadSacn::MatEmpty() const
{
	return m_img.empty();
}
