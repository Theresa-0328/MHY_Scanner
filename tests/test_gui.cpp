#include <gtest/gtest.h>

#include "QRScanner.h"

TEST(QRScanner, Test_ZZZ_qrcode_1440)
{
    cv::Mat image = cv::imread(R"(../doc/image/zzz_qrcode_1440.png)");
    QRScanner t;
    std::string qrcode1{};
    t.decodeSingle(image, qrcode1);
    const std::string& qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=12&app_name=%E7%BB%9D%E5%8C%BA%E9%9B%B6&bbs=true&biz_key=nap_cn&expire=1725289083&ticket=66cf3afb2a1c7a308832bd56)";
    EXPECT_EQ(qrcode1, qrcode2);
}

TEST(QRScanner, Test_bh3_qrcode_1080)
{
    cv::Mat image = cv::imread(R"(../doc/image/bh3_qrcode_1080.png)");
    QRScanner t;
    std::string qrcode1{};
    t.decodeSingle(image, qrcode1);
    const std::string& qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1683143216&ticket=644c22b06fd4de50d3fd5e36)";
    EXPECT_EQ(qrcode1, qrcode2);
}

TEST(QRScanner, Test2_hk4e_qrcode_1080)
{
    cv::Mat image = cv::imread(R"(../doc/image/hk4e_qrcode_1080.png)");
    QRScanner t;
    std::string qrcode1{};
    t.decodeSingle(image, qrcode1);
    const std::string& qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=4&app_name=%E5%8E%9F%E7%A5%9E&bbs=true&biz_key=hk4e_cn&expire=1690964505&ticket=64c37a9907698423ab04edd4)";
    EXPECT_EQ(qrcode1, qrcode2);
}

TEST(QRScanner, Test3_hkrpg_qrcode_1080)
{
    cv::Mat image = cv::imread(R"(../doc/image/hkrpg_qrcode_1080.png)");
    QRScanner t;
    std::string qrcode1{};
    t.decodeSingle(image, qrcode1);
    const std::string& qrcode2 = R"(https://user.mihoyo.com/qr_code_in_game.html?app_id=8&app_name=%E5%B4%A9%E5%9D%8F%3A%E6%98%9F%E7%A9%B9%E9%93%81%E9%81%93&bbs=true&biz_key=hkrpg_cn&expire=1686822569&ticket=6484472959a1066e0648ae67)";
    EXPECT_EQ(qrcode1, qrcode2);
}