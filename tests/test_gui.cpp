#include "gtest/gtest.h"
#include "ThreadSacn.h"

TEST(ThreadSacn, Test1)
{
	cv::Mat image = cv::imread(R"(../doc/image/bh3_qrcode_1080.png)");
	ThreadSacn t;
	t.setImg(image);
	t.start();
	t.wait();
	std::string qrcode1 = t.getQRcode();
	std::string qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1683143216&ticket=644c22b06fd4de50d3fd5e36)";
	EXPECT_EQ(qrcode1, qrcode2);
}

TEST(ThreadSacn, Test2)
{
	cv::Mat image = cv::imread(R"(../doc/image/hk4e_qrcode_1080.png)");
	ThreadSacn t;
	t.setImg(image);
	t.start();
	t.wait();
	std::string qrcode1 = t.getQRcode();
	std::string qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=4&app_name=%E5%8E%9F%E7%A5%9E&bbs=true&biz_key=hk4e_cn&expire=1690964505&ticket=64c37a9907698423ab04edd4)";
	EXPECT_EQ(qrcode1, qrcode2);
}

TEST(ThreadSacn, Test3)
{
	cv::Mat image = cv::imread(R"(../doc/image/hkrpg_qrcode_1080.png)");
	ThreadSacn t;
	t.setImg(image);
	t.start();
	t.wait();
	std::string qrcode1 = t.getQRcode();
	std::string qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=8&app_name=%E5%B4%A9%E5%9D%8F%3A%E6%98%9F%E7%A9%B9%E9%93%81%E9%81%93&bbs=true&biz_key=hkrpg_cn&expire=1686822569&ticket=6484472959a1066e0648ae67)";
	EXPECT_EQ(qrcode1, qrcode2);
}