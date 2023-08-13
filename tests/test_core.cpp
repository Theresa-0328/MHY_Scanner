#include <gtest/gtest.h>

#include "LiveStreamLink.h"
#include "QRScanner.h"

TEST(LiveBili, Test1)
{
	LiveBili liveBili("6");
	EXPECT_EQ(LiveStreamStatus::Status::Normal, liveBili.GetLiveStreamStatus());
	EXPECT_TRUE(!liveBili.GetLiveStreamLink().empty());
}

TEST(LiveBili, Test2)
{
	LiveBili liveBili("2");
	EXPECT_EQ(LiveStreamStatus::Status::Absent, liveBili.GetLiveStreamStatus());
}

TEST(LiveBili, Test3)
{
	LiveBili liveBili("2132121");
	EXPECT_EQ(LiveStreamStatus::Status::NotLive, liveBili.GetLiveStreamStatus());
}

TEST(LiveHuya, Test1)
{
	EXPECT_TRUE(true);
}

TEST(LiveDouyin, Test1)
{
	EXPECT_TRUE(true);
}

TEST(QRScanner, Test1)
{
	cv::Mat image = cv::imread(R"(../doc/image/bh3_qrcode_1080.png)");
	std::string qrcode1;
	QRScanner q;
	q.decodeSingle(image, qrcode1);
	std::string qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1683143216&ticket=644c22b06fd4de50d3fd5e36)";
	EXPECT_EQ(qrcode1, qrcode2);
}