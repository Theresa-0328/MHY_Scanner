#pragma once

#include <string>
#include <string_view>
#include <format>
#include <random>
#include <sstream>
#include <optional>

#include <Json.h>

#include "Common.h"
#include "HttpClient.h"
#include "UtilString.hpp"
#include "CryptoKit.h"

enum class QRCodeState : uint8_t
{
    Init = 0,
    Scanned = 1,
    Confirmed = 2,
    Expired = 3
};

constexpr std::string_view mihoyobbs_salt{ "oqrJbPCoFhWhFBNDvVRuldbrbiVxyWsP" };
constexpr std::string_view mihoyobbs_salt_web{ "zZDfHqEcwTqvvKDmqRcHyqqurxGgLfBV" };

constexpr std::string_view mihoyobbs_salt_x4{ "xV8v4Qu54lUKrEYFZkJhB8cuOh9Asafs" };
constexpr std::string_view mihoyobbs_salt_x6{ "t0qEgfub6cvueAPgR5m9aQWWVciEer7v" };

constexpr std::string_view mihoyobbs_verify_key{ "bll8iq97cem8" };

constexpr std::string_view mihoyobbs_version{ "2.75.2" };

[[nodiscard]] inline std::string DataSignAlgorithmVersionGen1()
{
    return "";
}

[[nodiscard]] inline std::string DataSignAlgorithmVersionGen2(const std::string_view body, const std::string_view query)
{
    const std::string time_now{ std::to_string(getCurrentUnixTime()) };
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    int lower_bound{ 100001 };
    int upper_bound{ 200000 };
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    const std::string rand{ std::to_string(dist(gen)) };
    std::string m{ "salt=" + std::string(mihoyobbs_salt_x6) + "&t=" + time_now + "&r=" + rand + "&b=" + std::string(body) + "&q=" + std::string(query) };
    return time_now + "," + rand + "," + Md5(m);
}

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

static GameType loginType{ GameType::TearsOfThemis };

inline std::string GetLoginQrcodeUrl(const std::string_view deviece, const GameType type = loginType)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, mhy_hk4e_qrcode_fetch, std::format(R"({{"app_id":{},"device":"{}"}})", static_cast<int>(type), deviece));
    json::Json j;
    j.parse(res);
    std::string QrcodeString{ static_cast<std::string>(j["data"]["url"]) };
    replace0026WithAmpersand(QrcodeString);
    return QrcodeString;
}

inline QRCodeState GetQRCodeState(const std::string_view deviece,
                                  const std::string_view ticket,
                                  std::string& accountData,
                                  const GameType type = loginType)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, mhy_hk4e_qrcode_query,
                  std::format(R"({{"app_id":{},"device":"{}","ticket":"{}"}})", static_cast<int>(type), deviece, ticket));
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

inline std::string getMysUserName(const std::string_view uid)
{
    std::string re;
    HttpClient h;
    h.GetRequest(re, std::format("{}?uid={}", mhy_mys_uesrinfo, uid).c_str());
    json::Json j;
    j.parse(re);
    re = j["data"]["user_info"]["nickname"];
    return re;
}

inline auto getStokenByGameToken(const std::string_view uid, const std::string_view game_token)
    -> std::optional<std::tuple<std::string, std::string>>
{
    static std::map<std::string, std::string> headers = {
        { "x-rpc-app_id", "bll8iq97cem8" },
        { "Referer", "https://app.mihoyo.com" },
        { "User-Agent", "Mozilla/5.0 (Linux; Android 12; LIO-AN00 Build/TKQ1.220829.002; wv) AppleWebKit/537.36 (KHTML, like Gecko) "
                        "Version/4.0 Chrome/103.0.5060.129 Mobile Safari/537.36 miHoYoBBS/2.67.1" }
    };
    std::string re;
    HttpClient h;
    h.PostRequest(re, mhy_takumi_game_token_stoken,
                  std::format("{{\"account_id\":{},\"game_token\":\"{}\"}}", uid, game_token),
                  headers);
    re = UTF8_To_string(re);
    json::Json j;
    j.parse(re);
    if (static_cast<int>(j["retcode"]) == 0)
    {
        return std::make_optional(std::make_tuple(std::move(static_cast<std::string>(j["data"]["user_info"]["mid"])),
                                                  std::move(static_cast<std::string>(j["data"]["token"]["token"]))));
    }
    else
    {
        return std::nullopt;
    }
}

inline bool scanQRLogin()
{
}

inline bool confirmQRLogin()
{
}
