#pragma once

#include <string>

#include <opencv2/wechat_qrcode.hpp>
#include <opencv2/opencv.hpp>

class QRScanner
{
public:
	QRScanner();
	~QRScanner();
	void decodeSingle(const cv::Mat& img, std::string& qrCode);
	void decodeMultiple(const cv::Mat& img, std::string& qrCode);
private:
	cv::Ptr<cv::wechat_qrcode::WeChatQRCode> detector;
};