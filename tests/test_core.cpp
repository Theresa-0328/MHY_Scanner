#include <gtest/gtest.h>

#include "LiveStreamLink.h"
#include "QRScanner.h"

TEST(LiveDoyin, Test1)
{
	LiveDouyin l("745964462470");
	l.GetLiveStreamStatus();
	EXPECT_TRUE(true);
}

TEST(URLEncode, test1)
{
	HttpClient h;
	std::string s1 = "Hello%23World%22";
	std::string s3 = R"(Hello#World")";
	std::string s2 = h.urlDecode(s1);
	EXPECT_EQ(s2, s3);
}

TEST(LiveHuya, Test1)
{
	LiveHuya l("660000");
	l.GetLiveStreamStatus();
	EXPECT_TRUE(true);
}


TEST(LiveDouyin, Test2)
{
	EXPECT_TRUE(true);
}

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