#include "OfficialApi.h"

#include <random>
#include <sstream>
#include <format>
#include <vector>
#include <algorithm>
#include <array>

#include <Json.h>

#include "CryptoKit.h"
#include "UtilString.hpp"
#include "MhyApi.hpp"

OfficialApi::OfficialApi() :
    scanUrl(nullptr),
    confirmUrl(nullptr)
{
}

void OfficialApi::scanInit(const GameType gameType, const std::string& ticket, const std::string& uid, const std::string& gameToken)
{
    switch (gameType)
    {
    case GameType::Honkai3:
        scanUrl = mhy_bh3_qrcode_scan;
        confirmUrl = mhy_bh3_qrcode_confirm;
        break;
    case GameType::Genshin:
        scanUrl = mhy_hk4e_qrcode_scan;
        confirmUrl = mhy_hk4e_qrcode_confirm;
        break;
    case GameType::HonkaiStarRail:
        scanUrl = mhy_hkrpg_qrcode_scan;
        confirmUrl = mhy_hkrpg_qrcode_confirm;
        break;
    case GameType::ZenlessZoneZero:
        break;
    default:
        __assume(0);
    }
    uuid = createUUID4();
    m_uid = uid;
    m_ticket = ticket;
    m_gameToken = gameToken;
    m_gameType = gameType;
}

std::string OfficialApi::getDS2()
{
    std::string time_now = std::to_string(getCurrentUnixTime());
    std::random_device rd;
    std::mt19937 gen(rd());
    // 定义范围
    int lower_bound = 100001;
    int upper_bound = 200000;
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    std::string rand = std::to_string(dist(gen));
    std::string m = "salt=" + salt + "&t=" + time_now + "&r=" + rand;
    Md5(m);
    return time_now + "," + rand + "," + m;
}

bool OfficialApi::validityCheck(std::string_view ticket)
{
    if (ticket == m_ticket)
    {
        return true;
    }
    return false;
}

//扫码请求
ScanRet OfficialApi::scanRequest()
{
    std::string m_sacnRet{};
    PostRequest(m_sacnRet,
                scanUrl,
                std::format(R"({{"app_id":{},"device":"{}","ticket":"{}"}})", static_cast<int>(m_gameType), uuid, m_ticket));
    json::Json j;
    j.parse(m_sacnRet);
    if ((int)j["retcode"] != 0)
    {
        return ScanRet::FAILURE_1;
    }
    return ScanRet::SUCCESS;
}

//扫码确认
ScanRet OfficialApi::scanConfirm()
{
    std::string s;
    json::Json payload;
    payload["proto"] = "Account";
    payload["raw"] = std::format(R"({{\"uid\":\"{}\",\"token\":\"{}\"}})", m_uid, m_gameToken);
    json::Json data;
    data["app_id"] = static_cast<int>(m_gameType);
    data["device"] = uuid;
    data["payload"] = payload;
    data["ticket"] = m_ticket;
    PostRequest(s, confirmUrl, data.str());
    json::Json j;
    j.parse(s);
    if ((int)j["retcode"] != 0)
    {
        return ScanRet::FAILURE_2;
    }
    return ScanRet::SUCCESS;
}

//获取用户完整信息
std::string OfficialApi::getUserName(const std::string& uid)
{
    std::string re;
    GetRequest(re, std::format("{}?uid={}", mhy_mys_uesrinfo, uid).c_str());
    json::Json j;
    j.parse(re);
    re = j["data"]["user_info"]["nickname"];
    return re;
}

//暂时用不上
std::string OfficialApi::getRole()
{
    std::string data;
    const char* url = "https://api-takumi.miyoushe.com/binding/api/getUserGameRolesByStoken";
    headers["DS"] = getDS2();
    headers["Cookie"] = "stuid=" + cookieMap.at("login_uid") + ";" + "stoken=" + data + ";" + "mid=" + "";
    std::string re;
    GetRequest(re, url, headers);
    headers.erase("Cookie");
    return std::string();
}

bool OfficialApi::cookieParser(const std::string& cookieString)
{
    if (cookieString == "")
    {
        return false;
    }
    // 切割 cookie 字符串
    size_t pos = 0;
    while (pos < cookieString.length())
    {
        size_t endPos = cookieString.find(';', pos);
        if (endPos == std::string::npos)
        {
            endPos = cookieString.length();
        }
        size_t equalPos = cookieString.find('=', pos);
        if (equalPos != std::string::npos && equalPos < endPos)
        {
            std::string key = cookieString.substr(pos, equalPos - pos);
            key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
            const std::string& value = cookieString.substr(equalPos + 1, endPos - equalPos - 1);
            cookieMap[key] = value;
        }
        pos = endPos + 1;
    }
    if ((cookieMap.count("stoken") > 0) &&
        ((cookieMap.count("stuid") > 0) || (cookieMap.count("ltuid")) || (cookieMap.count("account_id"))) &&
        cookieMap.count("mid"))
    {
        return true;
    }
    return false;
}

std::string OfficialApi::getStoken() const
{
    return cookieMap.at("stoken");
#if 0
    std::map<std::string, std::string> params = {
        { "login_ticket", cookieMap.at("login_ticket") },
        { "uid", cookieMap.at("login_uid") },
        { "token_types", "3" },
    };
    std::string s;
    GetRequest(s, std::format("{}?{}", mhy_takumi_multi_token_by_login_ticket, MapToQueryString(params)).c_str());
    json::Json j;
    j.parse(s);
    if ((int)j["retcode"] == 0)
    {
        data = j["data"]["list"][0]["token"];
        return 0;
    }
    else
    {
        data = j["message"];
        return -1;
    }
#endif
}

std::string OfficialApi::getUid() const
{
    std::array<std::string, 3> keys = { "stuid", "ltuid", "account_id" };
    auto it = std::find_if(keys.begin(), keys.end(), [&cookieMap = cookieMap](std::string key) {
        return cookieMap.find(key) != cookieMap.end();
    });
    if (it != keys.end())
    {
        return cookieMap.at(*it);
    }
}

std::string OfficialApi::getMid() const
{
    return cookieMap.at("mid");
}

int OfficialApi::getGameToken(const std::string& stoken, const std::string& uid, std::string& gameToken)
{
    std::map<std::string, std::string> params{
        { "stoken", stoken },
        { "mid", uid },
    };
    std::string s;
    GetRequest(s, std::format("{}?{}", mhy_takumi_game_token, MapToQueryString(params)).c_str());
    const std::string& data = UTF8_To_string(s);
    json::Json j;
    j.parse(data);
    int retcode = j["retcode"];
    if (retcode == 0)
    {
        gameToken = j["data"]["game_token"];
    }
    return retcode;
}