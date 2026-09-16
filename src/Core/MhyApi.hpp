#pragma once

#include <string>
#include <string_view>
#include <format>
#include <random>
#include <sstream>
#include <optional>
#include <iostream>

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "ApiDefs.hpp"
#include "CreateUUID.hpp"
#include "CryptoKit.h"
#include "UtilString.hpp"
#include "TimeStamp.hpp"

static const std::string device_id{ CreateUUID::CreateUUID4() };
static GameType loginType{ GameType::TearsOfThemis };

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

inline cpr::Header GetRequestHeader()
{
    static cpr::Header headers{
        { "User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) miHoYoBBS/2.76.1" },
        { "Accept", "application/json" },
        { "Content-Type", "application/json" },
        { "x-rpc-app_id", "bll8iq97cem8" },
        { "x-rpc-app_version", "2.76.1" },
        { "x-rpc-client_type", "2" },
        { "x-rpc-device_id", device_id },
        { "x-rpc-device_name", "" },
        { "x-rpc-game_biz", "bbs_cn" },
        { "x-rpc-sdk_version", "2.16.0" }
    };
    return headers;
}

struct QRLoginData
{
    std::string url;
    std::string ticket;
};

// 扫码登录专用请求头：app_id 必须用 dw9y09jqjpxc（对齐 1.16 C++），
// 这样 Confirmed 时 data.tokens 才会返回 stoken。
inline cpr::Header GetPassportQRHeader()
{
    return cpr::Header{
        { "Content-Type", "application/json" },
        { "x-rpc-app_id", "dw9y09jqjpxc" },
        { "x-rpc-device_id", device_id }
    };
}

// 1.16 新协议：passport createQRLogin 直接返回 url 与 ticket 两个字段
inline QRLoginData GetLoginQrcodeUrl(const GameType type = loginType)
{
    try
    {
        const auto response = cpr::Post(
            cpr::Url{ api::mhy::passport::create_qr_login },
            cpr::Body{ nlohmann::json::object().dump() },
            GetPassportQRHeader());

        const auto data = nlohmann::json::parse(response.text);
        if (data.value("retcode", -1) != 0)
            return {};
        const auto d = data.value("data", nlohmann::json::object());
        return { d.value("url", ""), d.value("ticket", "") };
    }
    catch (...)
    {
        return {};
    }
}

// 1.16 新协议：passport web 扫码状态轮询。
// 返回 (状态, uid, stoken, mid)。
// 返回 (status, uid, stoken, mid)。Confirmed 时：
//   stoken = data.tokens[0].token，uid/mid = data.user_info.aid/mid。
inline std::tuple<LoginQRCodeState, std::string, std::string, std::string> GetQRCodeState(
    const std::string_view ticket,
    const GameType type = loginType)
{
    try
    {
        const auto response = cpr::Post(
            cpr::Url{ api::mhy::passport::query_qr_login_status },
            cpr::Body{ nlohmann::json{ { "ticket", ticket } }.dump() },
            GetPassportQRHeader());

        const auto data = nlohmann::json::parse(response.text);
        if (data.value("retcode", -1) != 0)
            return { LoginQRCodeState::Expired, {}, {}, {} };

        const std::string status = data.value("data", nlohmann::json::object()).value("status", "");
        if (status == "Created")
            return { LoginQRCodeState::Init, {}, {}, {} };
        if (status == "Scanned")
            return { LoginQRCodeState::Scanned, {}, {}, {} };
        if (status == "Confirmed")
        {
            const auto d = data.value("data", nlohmann::json::object());
            const auto ui = d.value("user_info", nlohmann::json::object());

            std::string uid = ui.value("aid", "");
            std::string mid = ui.value("mid", uid);

            // stoken 在 data.tokens[0].token（Confirmed 时下发），而非 Set-Cookie。
            std::string stoken;
            const auto tokens = d.value("tokens", nlohmann::json::array());
            if (tokens.is_array() && !tokens.empty())
            {
                stoken = tokens[0].value("token", "");
            }

            return { LoginQRCodeState::Confirmed, uid, stoken, mid };
        }
        return { LoginQRCodeState::Expired, {}, {}, {} };
    }
    catch (...)
    {
        return { LoginQRCodeState::Expired, {}, {}, {} };
    }
}

inline std::string getMysUserName(const std::string_view uid)
{
    try
    {
        static constexpr std::string_view url = api::mhy::mys::userinfo;
        const auto response = cpr::Get(
            cpr::Url{ std::format("{}?uid={}", url, uid) });

        const auto data = nlohmann::json::parse(response.text);
        return data.value("data", nlohmann::json::object())
                   .value("user_info", nlohmann::json::object())
                   .value("nickname", std::string{ uid });
    }
    catch (...)
    {
        return std::string{ uid };
    }
}

inline bool CheckStokenValid(
    const std::string_view stoken,
    const std::string_view mid)
{
    if (stoken.empty() || mid.empty())
    {
        return false;
    }
    const auto response = cpr::Get(
        cpr::Url{ api::mhy::takumi::cookie_account_info_by_stoken },
        cpr::Header{
            { "Accept", "application/json" },
            { "Cookie", std::format("stoken={};mid={};", stoken, mid) } });

    const auto j = nlohmann::json::parse(response.text);
    return j.value("retcode", -1) == 0;
}

inline std::tuple<int, GeetestData> CreateLoginCaptcha(
    const std::string_view mobile,
    const std::string_view aigis = "")
{
    const std::string body{ nlohmann::json{
        { "area_code", Encrypt("+86") },
        { "mobile", Encrypt(mobile) } }
                                .dump() };
    cpr::Header reqHeaders{ GetRequestHeader() };
    reqHeaders["DS"] = DataSignAlgorithmVersionGen2(body, "");
    if (!aigis.empty())
        reqHeaders["X-Rpc-Aigis"] = aigis;
    const auto response = cpr::Post(
        cpr::Url{ api::mhy::passport::login_by_mobile_captcha },
        cpr::Body{ body },
        cpr::Header{ reqHeaders });

    const auto j = nlohmann::json::parse(response.text);
    const int retcode = j.value("retcode", -1);
    GeetestData result{};
    if (retcode == 0)
    {
        result.action_type = j["data"]["action_type"].get<std::string>();
        return { retcode, result };
    }
    if (retcode == -3101)
    {
        const auto it = response.header.find("X-Rpc-Aigis");
        if (it != response.header.end())
        {
            const auto aigisJson = nlohmann::json::parse(it->second);
            const auto captchaJson = nlohmann::json::parse(aigisJson["data"].get<std::string>());

            result.session_id = aigisJson["session_id"].get<std::string>();
            result.mmt_type = aigisJson["mmt_type"].get<int>();
            result.gt = captchaJson["gt"].get<std::string>();
            result.challenge = captchaJson["challenge"].get<std::string>();
            result.GeeTestType = ServerType::Official;
        }
    }
    return { retcode, result };
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
#if 0
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
#endif
    return result;
}

inline std::string PandaScanQRCode(const std::string_view scanUrl, const std::string_view ticket, GameType gameType)
{
    const auto response = cpr::Post(
        cpr::Url{ scanUrl },
        cpr::Body{ nlohmann::json{
            { "passport_app_id", "bll8iq97cem8" },
            { "ticket", ticket },
            { "app_id", static_cast<int>(gameType) },
            { "device", device_id },
            { "ts", GetUnixTimeStampSeconds() } }
                       .dump() },
        cpr::Header{
            { "Content-Type", "application/json" },
            { "x-rpc-app_id", "bll8iq97cem8" },
            { "x-rpc-device_id", device_id } });

    const auto j = nlohmann::json::parse(response.text);
    if (j.value("retcode", -1) != 0)
    {
        return {};
    }
    return j.value("data", nlohmann::json::object()).value("passport_qr_url", "");
}

inline bool PassportQRLogin(const std::string_view passportQRUrl, const std::string_view stoken, const std::string_view mid, const bool confirm)
{
    const std::string qrUrl{ passportQRUrl };
    const auto getParam = [&qrUrl](const std::string_view key, const char terminator) -> std::string
    {
        const std::string needle{ std::string{ key } + "=" };
        const auto b = qrUrl.find(needle);
        if (b == std::string::npos)
        {
            return {};
        }
        const auto vb = b + needle.size();
        const auto ve = qrUrl.find(terminator, vb);
        return qrUrl.substr(vb, ve == std::string::npos ? std::string::npos : ve - vb);
    };

    const std::string ticket = getParam("tk", '&');
    const std::string tokenTypes = getParam("token_types", '#');
    if (ticket.empty() || tokenTypes.empty())
    {
        return false;
    }

    const auto response = cpr::Post(
        cpr::Url{ confirm ? std::string_view{ api::mhy::passport::confirm_qr_login } : std::string_view{ api::mhy::passport::scan_qr_login } },
        cpr::Body{ nlohmann::json{
            { "ticket", ticket },
            { "token_types", nlohmann::json::array({ tokenTypes }) } }
                       .dump() },
        cpr::Header{
            { "Content-Type", "application/json" },
            { "x-rpc-app_id", "bll8iq97cem8" },
            { "x-rpc-device_id", device_id },
            { "Cookie", std::format("stoken={};mid={};", stoken, mid) } });

    const auto j = nlohmann::json::parse(response.text);
    return j.value("retcode", -1) == 0;
}

inline std::string makeSign(const nlohmann::json& data)
{
    std::string param;
    for (auto& [key, value] : data.items())
    {
        if (key == "sign")
            continue;
        const std::string strVal = value.is_string() ? value.get<std::string>() : value.dump();

        param += key + "=" + strVal + "&";
    }
    if (!param.empty())
        param.pop_back();
#ifdef _DEBUG
    std::cout << "make_param = " << param << std::endl;
#endif
    constexpr std::string_view key = "0ebc517adb1b62c6b408df153331f9aa";
    return HmacSha256(param, std::string(key));
}

inline std::string& getOAString()
{
    static std::string value = []() {
        const auto response = cpr::Get(cpr::Url{ "https://api.v6qbb.cloud/get_bh3_bilibili_oa" });
        if (response.text.empty())
            throw std::runtime_error("");
        return response.text;
    }();
    return value;
}

inline std::tuple<int, std::string, std::string, std::string> GetBH3ExternalLoginInfo(const std::string& uid, const std::string& access_key)
{
    const std::string bodyData = std::format(R"({{"access_key":"{}","uid":{}}})", access_key, uid);

    nlohmann::json body{
        { "device", "0000000000000000" },
        { "app_id", 1 },
        { "channel_id", 14 },
        { "data", bodyData }
    };
    body["sign"] = makeSign(body);
    const auto response = cpr::Post(
        cpr::Url{ api::mhy::bh3::v2_login },
        cpr::Header{ { "Content-Type", "application/json" } },
        cpr::Body{ body.dump() });

    const auto j = nlohmann::json::parse(response.text);
    const int retcode = j.value("retcode", -1);

#ifdef _DEBUG
    std::cout << "崩坏3验证完成 : " << response.text << std::endl;
#endif

    if (retcode != 0)
    {
        return { retcode, {}, {}, {} };
    }

    return { 0,
             j["data"]["open_id"].get<std::string>(),
             j["data"]["combo_token"].get<std::string>(),
             j["data"]["combo_id"].get<std::string>() };
}

inline ScanRet scanCheck(const std::string& ticket)
{
    const std::string body = nlohmann::json{
        { "app_id", "1" },
        { "device", "0000000000000000" },
        { "ticket", ticket },
        { "ts", GetUnixTimeStampSeconds() }
    }.dump();

    const auto response = cpr::Post(
        cpr::Url{ api::mhy::bh3::qrcode_scan },
        cpr::Body{ body },
        cpr::Header{ { "Content-Type", "application/json" } });

    const auto j = nlohmann::json::parse(response.text);
    return j.value("retcode", -1) == 0 ? ScanRet::SUCCESS : ScanRet::FAILURE_1;
}

inline ScanRet scanConfirm(const std::string& ticket, const std::string& uid, const std::string& access_key, const std::string& name)
{
    auto [code, open_id, combo_token, combo_id] = GetBH3ExternalLoginInfo(uid, access_key);
    if (code != 0)
        return ScanRet::FAILURE_2;

    const auto raw =
        nlohmann::json{
            { "heartbeat", false },
            { "open_id", open_id },
            { "device_id", "0000000000000000" },
            { "app_id", "1" },
            { "channel_id", "14" },
            { "combo_token", combo_token },
            { "asterisk_name", name },
            { "combo_id", combo_id },
            { "account_type", "2" }
        };

    const auto ext =
        nlohmann::json{
            { "data", nlohmann::json{
                          { "accountType", "2" },
                          { "accountID", "" },
                          { "c", open_id },
                          { "accountToken", combo_token },
                          { "dispatch", getOAString() } } }
        };

    const nlohmann::json postBody{
        { "device", "0000000000000000" },
        { "app_id", 1 },
        { "ts", GetUnixTimeStampSeconds() },
        { "ticket", ticket },
        { "payload", nlohmann::json{
                         { "proto", "Combo" },
                         { "raw", raw.dump() },
                         { "ext", ext.dump() } } }
    };

#ifdef _DEBUG
    std::cout << postBody.dump() << std::endl;
#endif

    const auto response = cpr::Post(
        cpr::Url{ api::mhy::bh3::qrcode_confirm },
        cpr::Header{ { "Content-Type", "application/json" } },
        cpr::Body{ postBody.dump() });

    const auto j = nlohmann::json::parse(response.text);
    return j.value("retcode", -1) == 0 ? ScanRet::SUCCESS : ScanRet::FAILURE_2;
}