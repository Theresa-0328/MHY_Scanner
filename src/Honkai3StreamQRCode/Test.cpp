#include <zbar.h>
#include <fstream>
#include "Login.h"
#include "Bsgsdk.h"
#include "CryptoKit.h"
#include "ScreenScan.h"
#include <windows.h>
#include <opencv2/opencv.hpp>

//https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5
int main()
{
	//{
	//	std::wstring wstr = L".\\dll\\opencv_world460.dll";
	//	//MessageBox(NULL, wstr.c_str(), L"Title", MB_OK);
	//	// 加载OpenCV库
	//	HINSTANCE hInst = LoadLibrary(wstr.c_str());
	//	if (!hInst)
	//	{
	//		MessageBox(NULL, L"Could not load opencv_world460.dll!", L"Error", MB_OK | MB_ICONERROR);
	//		return -1;
	//	}

	//	cv::Mat img = cv::imread("screenshot.jpg");
	//	if (img.empty())
	//		return -1;

	//	cv::imshow("image", img);
	//	cv::waitKey(0);

	//	// 释放OpenCV库
	//	FreeLibrary(hInst);
	//}
	
	Login login("config_private.json");
	login.signedIn();
	login.setName();
	login.bh3Info();
	login.putConfigFile();
	
	std::string deCode;
	//开始扫码
	ScreenScan screenshot;
	zbar::ImageScanner scanner;
	cv::Mat gray;
	cv::Mat img;
	cv::Mat img_;

	std::cout << "检测屏幕上的二维码中" << std::endl;
	while(true)
	{

		img = screenshot.getScreenshot();
		//img_ = screenshot.getScreenshot(1040, 132, 800, 880);//考虑低性能计算机
		//imwrite("screenshot.jpg", img);
		//imwrite("screenshot_part.jpg", img_);
		cv::cvtColor(img, gray, cv::COLOR_RGB2GRAY);
		zbar::Image image(gray.cols, gray.rows, "Y800", gray.data, gray.cols * gray.rows);
		scanner.scan(image);
		for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
		{
#ifdef _DEBUG
			std::cout << "Decoded: " << symbol->get_data() << std::endl;
#endif // DEBUG
			deCode = symbol->get_data();
		}
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			login.scanQRCode(deCode);
			break;
		}
		cv::waitKey(30);
	}
	getchar();
	return 0;
}