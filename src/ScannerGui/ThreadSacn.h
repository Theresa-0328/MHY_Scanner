#pragma once

#include <QThread>
#include <string>
#include "QRScanner.h"

class ServerType
{
public:
	enum Type
	{
		UNKNOW = 0,
		Official = 1,
		BH3_BiliBili = 2
	};
};

class ThreadSacn : public QThread
{
	Q_OBJECT
public:
	ThreadSacn();
	~ThreadSacn();
	void setImg(const cv::Mat& img);
	//调用前确保成员变量uqrcode存在ticket
	std::string getTicket();
	void run();
	std::string uqrcode;
private:
	cv::Mat img;
	QRScanner qs;
};