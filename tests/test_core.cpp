﻿#include <gtest/gtest.h>

#include "LiveStreamLink.h"
#include "QRScanner.h"
#include "UtilString.hpp"
#include "MhyApi.hpp"

TEST(API, Test2)
{
    HttpClient h;
    std::string s{};
    h.GetRequest(s, "https://techain.baidu.com/srcmon");
}

TEST(API, Test1)
{
    auto result = getStokenByGameToken("", "");
    EXPECT_TRUE(result);
}

TEST(UUID, Test1)
{
    std::string s{ CreateUUID::CreateUUID4() };
    EXPECT_TRUE(s.size() > 35);
}

TEST(URLEncode, test1)
{
    HttpClient h;
    std::string s1 = "Hello%23World%22";
    std::string s3 = R"(Hello#World")";
    std::string s2 = urlDecode(s1);
    EXPECT_EQ(s2, s3);
}