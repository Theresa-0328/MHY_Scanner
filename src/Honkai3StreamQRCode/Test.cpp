#include <zbar.h>
#include <fstream>
#include "Bsgsdk.h"
#include "Mihoyosdk.h"
#include "CryptoKit.h"
#include "ScreenScan.h"
using namespace json;
using namespace std;


int main()
{
	
	std::ifstream inFile;
	json::Json config;
	std::stringstream configStringStream;
	inFile.open("config_private.json");
	if (inFile) 
	{ 
		configStringStream << inFile.rdbuf();
		const std::string& configString = configStringStream.str();
		config.parse(configString);
	}
	else 
	{
		inFile.open("config.json");
		configStringStream << inFile.rdbuf();
		const std::string& configString = configStringStream.str();
		config.parse(configString);
	}

	Bsgsdk b;
	Json j;
	Mihoyosdk m;
	Json loginJ;
	loginJ.parse(b.login1(config["account"], config["password"]));
	string a1 = loginJ["uid"].str();
	string a2 = loginJ["access_key"];
	loginJ.clear();
	j = b.getUserInfo(a1, a2);
	int uid = std::stoi(j["uid"]);
	string access_key = j["access_key"];
	std::string bhInfo = m.verify(uid, access_key);
	//登录成功！
	m.getOAServer();
	m.setUserName("测试名称test001");
	//开始扫码
	std::string deCode = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5)";
	m.scanCheck(deCode,bhInfo);
	loginJ.clear();
	j.clear();
	
	deCode.clear();
	ScreenScan screenshot;
	zbar::ImageScanner scanner;
	cv::Mat gray;
	cv::Mat img;
	cv::Mat img_;

	cout << "检测屏幕上的二维码中" << endl;
	while(1)
	{

		img = screenshot.getScreenshot();
		img_ = screenshot.getScreenshot(1040, 132, 800, 880);
		//imwrite("screenshot.jpg", img);
		//imwrite("screenshot_part.jpg", img_);
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		zbar::Image image(gray.cols, gray.rows, "Y800", gray.data, gray.cols * gray.rows);
		scanner.scan(image);
		for (zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
		{
			std::cout << "Decoded: " << symbol->get_data() << std::endl;
			deCode = symbol->get_data();
		}
		if (deCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			m.scanCheck(deCode, bhInfo);
		}
		cv::waitKey(16);
	}
	return 0;
}