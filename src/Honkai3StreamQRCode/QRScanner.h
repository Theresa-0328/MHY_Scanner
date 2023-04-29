#pragma once
#include <string>
#include <opencv2/wechat_qrcode.hpp>
#include <opencv2/opencv.hpp>

class QRScanner
{
public:
	QRScanner();
	void Decode(cv::Mat img, std::string& qrCode);
private:
	cv::Ptr<cv::wechat_qrcode::WeChatQRCode> detector;
	std::string detect_prototxt = "./ScanModel/detect.prototxt";
	std::string detect_caffe_model = "./ScanModel/detect.caffemodel";
	std::string sr_prototxt = "./ScanModel/sr.prototxt";
	std::string sr_caffe_model = "./ScanModel/sr.caffemodel";
};
//{
//	std::wstring wstr = L"./dll/opencv_world460.dll";
//	//MessageBox(NULL, wstr.c_str(), L"Title", MB_OK);
//	// º”‘ÿOpenCVø‚
//	HINSTANCE hInst = LoadLibrary(wstr.c_str());
//	//  Õ∑≈OpenCVø‚
//	FreeLibrary(hInst);
//}