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
#include "CreateUUID.hpp"

enum class QRCodeState : uint8_t
{
    Init = 0,
    Scanned = 1,
    Confirmed = 2,
    Expired = 3
};

constinit const std::string_view mihoyobbs_salt{ "oqrJbPCoFhWhFBNDvVRuldbrbiVxyWsP" };
constinit const std::string_view mihoyobbs_salt_web{ "zZDfHqEcwTqvvKDmqRcHyqqurxGgLfBV" };

constinit const std::string_view mihoyobbs_salt_x4{ "xV8v4Qu54lUKrEYFZkJhB8cuOh9Asafs" };
constinit const std::string_view mihoyobbs_salt_x6{ "t0qEgfub6cvueAPgR5m9aQWWVciEer7v" };

constinit const std::string_view mihoyobbs_version{ "2.75.2" };

static const std::string device_id{ CreateUUID::CreateUUID4() };

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
    static const std::map<std::string, std::string> header{
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) miHoYoBBS/2.76.1" },
        { "Accept", "application/json" },
        { "x-rpc-app_id", "bll8iq97cem8" },
        { "x-rpc-app_version", "2.76.1" },
        { "x-rpc-client_type", "2" },
        { "x-rpc-device_id", device_id },
        { "x-rpc-device_name", "" },
        { "x-rpc-game_biz", "bbs_cn" },
        { "x-rpc-sdk_version", "2.16.0" }

    };
    return header;
}

static GameType loginType{ GameType::TearsOfThemis };

inline std::string GetLoginQrcodeUrl(const GameType type = loginType)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, mhy_hk4e_qrcode_fetch, std::format(R"({{"app_id":{},"device":"{}"}})", static_cast<int>(type), device_id));
    json::Json j;
    j.parse(res);
    std::string QrcodeString{ static_cast<std::string>(j["data"]["url"]) };
    replace0026WithAmpersand(QrcodeString);
    return QrcodeString;
}

inline auto GetQRCodeState(
    const std::string_view ticket,
    const GameType type = loginType)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, mhy_hk4e_qrcode_query,
                  std::format(R"({{"app_id":{},"device":"{}","ticket":"{}"}})", static_cast<int>(type), device_id, ticket));
    //std::cout << __LINE__ << res << std::endl;
    json::Json j;
    j.parse(res);
    struct
    {
        enum
        {
            Init = 0,
            Scanned = 1,
            Confirmed = 2,
            Expired = 3
        } StateType;
        int retcode{};
        std::string uid{};
        std::string token{};
    } result;
    if (static_cast<int>(j["retcode"]) == 0)
    {
        if (static_cast<std::string>(j["data"]["stat"]) == "Init")
        {
            result.StateType = result.Init;
        }
        else if (static_cast<std::string>(j["data"]["stat"]) == "Scanned")
        {
            result.StateType = result.Scanned;
        }
        else if (static_cast<std::string>(j["data"]["stat"]) == "Confirmed")
        {
            std::string str{ unescapeString(j["data"]["payload"]["raw"]) };
            j.parse(str);
            result.uid = j["uid"];
            result.token = j["token"];
            result.StateType = result.Confirmed;
        }
    }
    else //retcode == -106
    {
        result.StateType = result.Expired;
    }
    return result;
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
    static constinit const char* PublicKey{
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
    //std::cout << s << std::endl;
    std::string bodystr{};
    if (size_t startPos = s.find_last_of("\n"); startPos != std::string::npos)
    {
        bodystr = s.substr(startPos + 1, bodystr.size() - startPos);
    }
    json::Json body{};
    body.parse(bodystr);
    GeetestData.retcode = body["retcode"];
    if (GeetestData.retcode == 0)
    {
        GeetestData.retcode = body["retcode"];
        GeetestData.action_type = body["data"]["action_type"];
    }
    else if (GeetestData.retcode == -3006)
    {
        return GeetestData;
    }
    else if (GeetestData.retcode == -3101)
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

inline auto LoginByMobileCaptcha(const std::string_view actionType, const std::string_view mobile, const std::string_view captcha, const std::string_view aigis = "")
{
    struct
    {
        int retcode{};
        struct
        {
            std::string V2Token{};
            std::string aid{};
            std::string mid{};
        } data;
    } result;
    const std::string RequestBody{ std::format(R"({{"area_code":"{}","action_type":"{}","captcha":"{}","mobile":"{}"}})", Encrypt("+86"), actionType, captcha, Encrypt(mobile)) };
    std::map<std::string, std::string> headers{ GetRequestHeader() };
    headers["DS"] = DataSignAlgorithmVersionGen2(RequestBody, "");
    if (!aigis.empty())
    {
        headers["X-Rpc-Aigis"] = aigis;
    }
    HttpClient h;
    std::string s;
    h.PostRequest(s, URL_LoginByMobileCaptcha, RequestBody, headers);
    //std::cout << s << std::endl;
    json::Json j{};
    j.parse(s);
    result.retcode = j["retcode"];
    if (result.retcode == -3205)
    {
        return result;
    }
    else if (result.retcode == 0)
    {
        result.data.V2Token = j["data"]["token"]["token"];
        result.data.aid = j["data"]["user_info"]["aid"];
        result.data.mid = j["data"]["user_info"]["mid"];
    }
    return result;
}

inline bool scanQRLogin()
{
}

inline bool confirmQRLogin()
{
}
