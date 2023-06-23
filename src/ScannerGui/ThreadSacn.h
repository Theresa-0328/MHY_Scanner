#pragma once

#include <QThread>
#include <string>
#include "QRScanner.h"

class ThreadSacn  : public QThread
{
	Q_OBJECT

public:
	ThreadSacn();
	~ThreadSacn();
	void setImg(cv::Mat img);
	std::string getTicket();
	void run();
	std::string uqrcode;
private:
	cv::Mat img;
	QRScanner qs;
};