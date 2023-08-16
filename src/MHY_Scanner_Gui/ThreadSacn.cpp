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
	m_stop = false;
	this->wait();

}

void ThreadSacn::setImg(const cv::Mat& img)
{
	if (m_mux.tryLock())
	{
		this->m_img = img;
		m_mux.unlock();
	}
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
	cv::namedWindow("Video_Stream", cv::WINDOW_NORMAL);
	cv::resizeWindow("Video_Stream", 1280, 720);
	m_stop = true;
	while (m_stop)
	{
		if (!MatEmpty())
		{
#ifdef _DEBUG
			auto startTime = std::chrono::high_resolution_clock::now();
#endif // _DEBUG
			decodeMultiple(m_img, m_qrcode);
			cv::imshow("Video_Stream", m_img);
			cv::waitKey(1);
			m_img.release();
#ifdef _DEBUG
			auto endTime = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
			std::cout << (double)duration / 1000000 << std::endl;
#endif // _DEBUG
		}
		else
		{
			cv::waitKey(10);
		}
	}
	cv::destroyWindow("Video_Stream");
}

void ThreadSacn::stop()
{
	m_stop = false;
}

bool ThreadSacn::MatEmpty() const
{
	return m_img.empty();
}
