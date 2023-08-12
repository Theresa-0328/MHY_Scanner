#pragma once

#include <string>

#include <QThread>

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
private:
	cv::Mat img;
	std::string m_qrcode;
};