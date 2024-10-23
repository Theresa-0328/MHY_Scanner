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
#include "TimeStamp.hpp"

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

constexpr std::string_view mihoyobbs_version{ "2.75.2" };

[[nodiscard]] inline std::string DataSignAlgorithmVersionGen1()
{
    return "";
}

[[nodiscard]] inline std::string DataSignAlgorithmVersionGen2(const std::string_view body, const std::string_view query)
{
    const std::string time_now{ std::to_string(GetUnixTimeStampSeconds()) };
    std::random_device rd{};
    std::mt19937 gen{ rd() };
    int lower_bound{ 100001 };
    int upper_bound{ 200000 };
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    const std::string rand{ std::to_string(dist(gen)) };
    std::string m{ "salt=" + std::string(mihoyobbs_salt_x6) + "&t=" + time_now + "&r=" + rand + "&b=" + std::string(body) + "&q=" + std::string(query) };
    return time_now + "," + rand + "," + Md5(m);
}

inline std::map<std::string, std::string> GetRequestHeader()
{
    static std::map<std::string, std::string> header{
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) miHoYoBBS/2.76.1" },
        { "Accept", "application/json" },
        { "x-rpc-app_id", "bll8iq97cem8" },
        { "x-rpc-app_version", "2.76.1" },
        { "x-rpc-client_type", "2" },
        { "x-rpc-device_id", "62bc0778-126f-4c63-be81-d5a86a6ba9b9" },
        { "x-rpc-device_name", "" },
        { "x-rpc-game_biz", "bbs_cn" },
        { "x-rpc-sdk_version", "2.16.0" }

    };
    return header;
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

inline std::string Encrypt(const std::string_view source)
{
    constexpr const char* PublicKey{
        "-----BEGIN PUBLIC KEY-----\n"
        "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDDvekdPMHN3AYhm/vktJT+YJr7"
        "cI5DcsNKqdsx5DZX0gDuWFuIjzdwButrIYPNmRJ1G8ybDIF7oDW2eEpm5sMbL9zs"
        "9ExXCdvqrn51qELbqj0XxtMTIpaCHFSI50PfPpTFV9Xt/hmyVwokoOXFlAEgCn+Q"
        "CgGs52bFoYMtyi+xEQIDAQAB\n"
        "-----END PUBLIC KEY-----"
    };
    return rsaEncrypt(source.data(), PublicKey);
}

inline auto CreateLoginCaptcha(const std::string_view mobile, const std::string_view aigis = "")
{
    struct
    {
        int retcode{};
        std::string action_type{};
        std::string session_id{};
        int mmt_type{};
        std::string gt{};
        std::string challenge{};
    } GeetestData;

    const std::string RequestBody{ std::format(R"({{"area_code":"{}","mobile":"{}"}})", Encrypt("+86"), Encrypt(mobile)) };
    std::map<std::string, std::string> headers{ GetRequestHeader() };
    headers["DS"] = DataSignAlgorithmVersionGen2(RequestBody, "");
    if (!aigis.empty())
    {
        headers["X-Rpc-Aigis"] = aigis;
    }
    HttpClient h;
    std::string s;
    h.PostRequest(s, mhy_passport_account_verifier, RequestBody, headers, true);
    std::string bodystr{};
    if (size_t startPos = s.find_last_of("\n"); startPos != std::string::npos)
    {
        bodystr = s.substr(startPos + 1, bodystr.size() - startPos);
    }
    json::Json body{};
    body.parse(bodystr);
    if ((int)body["retcode"] == 0)
    {
        GeetestData.retcode = body["retcode"];
        GeetestData.action_type = body["data"]["action_type"];
    }
    else
    {
        constexpr std::string_view headerKey{ "X-Rpc-Aigis: " };
        std::string Aigis;
        if (size_t startPos = s.find(headerKey); startPos != std::string::npos)
        {
            startPos += headerKey.length();
            size_t endPos = s.find("\n", startPos);
            Aigis = s.substr(startPos, endPos - startPos);
        }
        json::Json j1{};
        j1.parse(Aigis);
        std::string data{ j1["data"] };
        data = unescapeString(data);
        json::Json j2{};
        j2.parse(data);

        json::Json body{};
        body.parse(bodystr);
        GeetestData.retcode = body["retcode"];
        GeetestData.session_id = j1["session_id"];
        GeetestData.mmt_type = j1["mmt_type"];
        GeetestData.gt = j2["gt"];
        GeetestData.challenge = j2["challenge"];
    }
    return GeetestData;
}

inline bool LoginByMobileCaptcha()
{
}

inline bool scanQRLogin()
{
}

inline bool confirmQRLogin()
{
}
