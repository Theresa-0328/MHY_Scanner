#include "QRScanner.h"

QRScanner::QRScanner()
{
	detector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(detect_prototxt, detect_caffe_model, sr_prototxt, sr_caffe_model);
}

std::string QRScanner::Decode(cv::Mat img)
{
	std::vector<cv::String> strDecoded;
	std::string qrCode;
	strDecoded = detector->detectAndDecode(img);
	for (int i = 0; i < strDecoded.size(); i++)
	{
		qrCode = strDecoded[i];
//#ifdef _DEBUG
		std::cout << "decode:" << strDecoded[i] << std::endl;
//#endif // DEBUG
	}
	return qrCode;
}
