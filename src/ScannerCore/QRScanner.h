#pragma once
#include <string>
#include <opencv2/wechat_qrcode.hpp>
#include <opencv2/opencv.hpp>

class QRScanner
{
public:
	QRScanner();
	void decodeSingle(const cv::Mat& img, std::string& qrCode);
	void decodeMultiple(const cv::Mat& img, std::string& qrCode);
private:
	cv::Ptr<cv::wechat_qrcode::WeChatQRCode> detector;
	const std::string detect_prototxt = "./ScanModel/detect.prototxt";
	const std::string detect_caffe_model = "./ScanModel/detect.caffemodel";
	const std::string sr_prototxt = "./ScanModel/sr.prototxt";
	const std::string sr_caffe_model = "./ScanModel/sr.caffemodel";
	std::vector<cv::String> strDecoded;
};