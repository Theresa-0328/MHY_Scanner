#pragma once

#include <string>

#include <QThread>
#include <QMutex>

#include "QRScanner.h"

class ThreadSacn : public QThread, public QRScanner
{
	Q_OBJECT
public:
	ThreadSacn();
	~ThreadSacn();
	void setImg(const cv::Mat& img);
	//调用前必须确保成员变量m_qrcode包含子字符串ticket
	std::string getTicket()const;
	std::string getQRcode()const;
	void run();
	void stop();
	bool MatEmpty()const;
private:
	cv::Mat m_img;
	std::string m_qrcode;
	QMutex m_mux;
	bool m_stop = false;
};