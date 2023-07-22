#include "QRScanner.h"

QRScanner::QRScanner()
{
	detector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(detect_prototxt, detect_caffe_model, sr_prototxt, sr_caffe_model);
}

void QRScanner::decodeSingle(const cv::Mat& img, std::string& qrCode)
{
	strDecoded = detector->detectAndDecode(img);
	if (strDecoded.size() > 0)
	{
		qrCode = strDecoded[0];
	}
#ifdef _DEBUG
	std::cout << "decode:" << qrCode << std::endl;
#endif // DEBUG
}

void QRScanner::decodeMultiple(const cv::Mat& img, std::string& qrCode)
{
	strDecoded = detector->detectAndDecode(img);
	for (int i = 0; i < strDecoded.size(); i++)
	{
		qrCode = strDecoded[i];
#ifdef _DEBUG
		std::cout << "decode:" << qrCode << std::endl;
#endif // DEBUG
	}
}