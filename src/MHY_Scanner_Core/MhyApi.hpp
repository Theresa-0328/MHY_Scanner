#pragma once

#include <string>
#include <string_view>
#include <format>
#include <random>
#include <sstream>

#include <Json.h>

#include "Common.h"
#include "HttpClient.h"

enum class QRCodeState : uint8_t
{
    Init = 0,
    Scanned = 1,
    Confirmed = 2,
    Expired = 3
};

[[nodiscard]] inline std::string createUUID4()
{
    static const char chars[] = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);
    constexpr int UUID_LENGTH = 36;
    char uuid[UUID_LENGTH + 1]{};
    for (int i = 0; i < UUID_LENGTH; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            uuid[i] = '-';
            continue;
        }
        int randomDigit = distribution(generator);
        uuid[i] = chars[randomDigit];
    }
    uuid[UUID_LENGTH] = '\0';
    return std::string(uuid);
}

inline std::string GetLoginQrcodeUrl(const std::string_view deviece, const int gameTypeID = 4)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, hk4e_qrcode_fetch, std::format(R"({{"app_id":{},"device":"{}"}})", gameTypeID, deviece));
    json::Json j;
    j.parse(res);
    return static_cast<std::string>(j["data"]["url"]);
}

inline QRCodeState GetQRCodeState(const std::string_view deviece,
                                  const std::string_view ticket,
                                  std::string& accountData,
                                  const int gameTypeID = 4)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, hk4e_qrcode_query,
                  std::format(R"({{"app_id":{},"device":"{}","ticket":"{}"}})", gameTypeID, deviece, ticket));
    //std::cout << __LINE__ << res << std::endl;
    json::Json j;
    j.parse(res);
    QRCodeState state{};
    using enum QRCodeState;
    if (static_cast<int>(j["retcode"]) == 0)
    {
        if (static_cast<std::string>(j["data"]["stat"]) == "Init")
        {
            state = Init;
        }
        else if (static_cast<std::string>(j["data"]["stat"]) == "Scanned")
        {
            state = Scanned;
        }
        else if (static_cast<std::string>(j["data"]["stat"]) == "Confirmed")
        {
            accountData = j["data"]["payload"]["raw"];
            state = Confirmed;
        }
    }
    else //retcode == -106
    {
        state = Expired;
    }
    return state;
}

inline std::string getMysUserName(const std::string& uid)
{
    std::string re;
    HttpClient h;
    h.GetRequest(re, std::format("{}?uid={}", mhy_mys_uesrinfo, uid).c_str());
    json::Json j;
    j.parse(re);
    re = j["data"]["user_info"]["nickname"];
    return re;
}

inline std::string getStokenByGameToken(const std::string_view uid, const std::string_view game_token)
{
    return std::string();
}