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
        scanUrl = mhy_nap_cn_qrcode_scan;
        confirmUrl = mhy_nap_cn_qrcode_confirm;
        break;
    default:
        __assume(0);
    }
    uuid = CreateUUID::CreateUUID4();
    m_uid = uid;
    m_ticket = ticket;
    m_gameToken = gameToken;
    m_gameType = gameType;
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

//暂时用不上
std::string OfficialApi::getRole()
{
    std::string data;
    const char* url = "https://api-takumi.miyoushe.com/binding/api/getUserGameRolesByStoken";
    //headers["DS"] = getDS2();
    headers["Cookie"] = "stuid=" + cookieMap.at("login_uid") + ";" + "stoken=" + data + ";" + "mid=" + "";
    std::string re;
    GetRequest(re, url, headers);
    headers.erase("Cookie");
    return std::string();
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