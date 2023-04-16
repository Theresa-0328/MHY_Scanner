#include <zbar.h>
#include <fstream>
#include "Login.h"
#include "Bsgsdk.h"
#include "Mihoyosdk.h"
#include "CryptoKit.h"
#include "ScreenScan.h"
using namespace json;
using namespace std;

int main()
{
	Login login("config_private.json");
	login.signedIn();
	login.setName();
	login.bh3Info();
	login.putconfigFile();

	//configJson["setname"] = c.string_To_UTF8("∞Æ¿Úœ£—≈");
	//string configString = c.UTF8_To_string(configJson.str());
	//config.putconfigFile(configJson.str());
	//cout << configJson.str()<< endl;
	//config.putconfigFile(configJson.str());
	
	std::string deCode = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5)";
	deCode.clear();
	
	//ø™ º…®¬Î
	ScreenScan screenshot;
	zbar::ImageScanner scanner;
	cv::Mat gray;
	cv::Mat img;
	cv::Mat img_;

	std::cout << "ºÏ≤‚∆¡ƒª…œµƒ∂˛Œ¨¬Î÷–" << endl;
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
			login.scanQRCode(deCode);
		}
		cv::waitKey(16);
	}
	return 0;
}