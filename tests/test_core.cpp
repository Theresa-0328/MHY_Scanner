#include "gtest/gtest.h"
#include "LiveBili.h"
#include "QRScanner.h"

TEST(LiveBili, Test1)
{
	v2api v;
	EXPECT_EQ(7734200, v.GetRealRoomID("6"));
}

TEST(LiveHuya, Test1)
{
	EXPECT_TRUE(true);
}

TEST(LiveDouyin, Test1)
{
	EXPECT_TRUE(true);
}

TEST(QRScanner1, Test1)
{
	cv::Mat image = cv::imread(R"(../doc/image/bh3_qrcode_1080.png)");
	//cv::imshow("vv", image);
	//cv::waitKey(0);
	std::string qrcode1;
	QRScanner q;
	q.decodeSingle(image, qrcode1);
	std::string qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1683143216&ticket=644c22b06fd4de50d3fd5e36)";
	EXPECT_EQ(qrcode1, qrcode2);
}