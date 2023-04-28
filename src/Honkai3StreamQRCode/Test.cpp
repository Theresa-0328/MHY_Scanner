#include <fstream>
#include <windows.h>
#include "Login.h"
#include "Bsgsdk.h"
#include "CryptoKit.h"
#include "ScreenScan.h"
#include "QRScanner.h"

//https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5
int main()
{
	//{
	//	std::wstring wstr = L"./dll/opencv_world460.dll";
	//	//MessageBox(NULL, wstr.c_str(), L"Title", MB_OK);
	//	// 加载OpenCV库
	//	HINSTANCE hInst = LoadLibrary(wstr.c_str());
	//	// 释放OpenCV库
	//	FreeLibrary(hInst);
	//}
	Login login("config_private.json");
	login.signedIn();
	login.setName();
	login.bh3Info();
	login.putConfigFile();
	//开始扫码
	ScreenScan screenshot;
	cv::Mat img;
	QRScanner s;
	std::cout << "检测屏幕上的二维码中" << std::endl;
	while(true)
	{
		std::string deCode;
		img = screenshot.getScreenshot();
		deCode = s.Decode(img);
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			login.scanQRCode(deCode);
			break;
		}
		cv::waitKey(60);
	}
	getchar();
	return 0;
}