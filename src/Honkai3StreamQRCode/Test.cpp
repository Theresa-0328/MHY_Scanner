#include "Login.h"
#include "Bsgsdk.h"
#include "CryptoKit.h"
#include "ScreenScan.h"
#include "QRScanner.h"

int main()
{
	//加载配置文件并登录
	Login login("config_private.json");
	login.signedIn();
	login.setName();
	login.bh3Info();
	//更新配置文件
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
		//img = screenshot.getScreenshot(600,250,600,600);
		s.Decode(img, deCode);
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			login.scanQRCode(deCode);
			break;
		}
		cv::waitKey(100);
	}
	getchar();
	return 0;
}