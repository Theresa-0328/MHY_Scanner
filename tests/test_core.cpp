#include "gtest/gtest.h"
#include "LiveBili.h"

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