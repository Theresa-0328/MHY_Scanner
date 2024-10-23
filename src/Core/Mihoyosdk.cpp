#include "Mihoyosdk.h"

#include <Json.h>
#include <format>

#include <Json.h>

#include "CryptoKit.h"
#include "UtilString.hpp"
#include "TimeStamp.hpp"

Mihoyosdk::Mihoyosdk()
{
}

std::string Mihoyosdk::verify(const std::string& uid, const std::string& access_key)
{
#ifdef _DEBUG
    std::cout << "verify with uid = " << uid << std::endl;
#endif // _DEBUG
    verifyData["uid"] = uid;
    verifyData["access_key"] = access_key;
    const std::string bodyData = std::format(R"({{\"access_key\":\"{}\",\"uid\":{}}})", verifyData["access_key"], verifyData["uid"]);
    json::Json body;
    body.parse(verifyBody);
    body["data"] = bodyData;
    const std::string sBody = body.str();
    body.clear();
    std::string s;
    PostRequest(s, mhy_bh3_v2_login, makeSign(sBody));
#ifdef _DEBUG
    std::cout << "崩坏3验证完成 : " << s << std::endl;
#endif // _DEBUG
    return s;
}

void Mihoyosdk::setBHVer(const std::string& s)
{
    bh3Ver = s;
}

void Mihoyosdk::setOAServer()
{
    oaString = getOAServer();
}

bool Mihoyosdk::validityCheck(std::string_view ticket)
{
    if (ticket == m_ticket)
    {
        return true;
    }
    return false;
}

std::string Mihoyosdk::getOAServer()
{
    //const std::string bhVer = bh3Ver;
    //const std::string oaMainUrl = "https://dispatch.scanner.hellocraft.xyz/v1/query_dispatch/?version=";
    //std::string param = bhVer + "_gf_android_bilibili&t=" + std::to_string(getCurrentUnixTime());
    //std::string feedback;
    //GetRequest(feedback, oaMainUrl + param);

    std::string oaserver;
    GetRequest(oaserver, "https://mi-m-cpjgtouitx.cn-hangzhou.fcapp.run");
    if (oaserver.empty())
    {
        exit(0);
    }
    return oaserver;

    //	param = bhVer + "_gf_android_bilibili&t=" + std::to_string(getCurrentUnixTime());
    //	json::Json j;
    //	j.parse(feedback);
    //	std::string dispatch_url = j["region_list"][0]["dispatch_url"];
    //	std::string dispatch;
    //	PostRequest(dispatch, dispatch_url + param, "");
    //	dispatch = UTF8_To_string(dispatch);
    //#ifdef _DEBUG
    //	std::cout << "获得OA服务器 : " << dispatch << std::endl;
    //#endif // DEBUG
    //	j.clear();
    //	return dispatch;
}

void Mihoyosdk::scanInit(const std::string& ticket, const std::string& bhInfo)
{
    m_ticket = ticket;
    m_bhInfo = bhInfo;
}

ScanRet Mihoyosdk::scanCheck()
{
    json::Json check;
    check.parse(scanCheckS);
    check["ticket"] = m_ticket;
    check["ts"] = (int)GetUnixTimeStampSeconds();
    std::string postBody = makeSign(check.str());
    std::string feedback;
    PostRequest(feedback, mhy_bh3_qrcode_scan, postBody); //扫码请求
    check.parse(feedback);
    int retcode = (int)check["retcode"];
    check.clear();
    if (retcode != 0)
    {
        return ScanRet::FAILURE_1;
    }
    return ScanRet::SUCCESS;
}

ScanRet Mihoyosdk::scanConfirm()
{
    json::Json bhInfoJ;
    bhInfoJ.parse(m_bhInfo);

    json::Json bhInFo;
    bhInFo.parse(bhInfoJ["data"].str());
    //std::cout << bhInFo.str() << std::endl;
    json::Json scanResultJ;
    scanResultJ.parse(scanResult);

    json::Json scanDataJ;
    scanDataJ.parse(scanData);

    //json::Json oa;
    //oa.parse(oaString);
    scanDataJ["dispatch"] = oaString;
    scanDataJ["c"] = bhInFo["open_id"];
    std::cout << std::format("{} \n", (std::string)bhInFo["open_id"]);
    scanDataJ["accountToken"] = bhInFo["combo_token"];
    //std::cout << scanDataJ.str() << std::endl;
    json::Json scanExtJ;
    scanExtJ.parse(scanExtR);
    scanExtJ["data"] = scanDataJ;
    json::Json scanRawJ;
    scanRawJ.parse(scanRawR);
    scanRawJ["open_id"] = bhInFo["open_id"];
    scanRawJ["combo_id"] = bhInFo["combo_id"];
    scanRawJ["combo_token"] = bhInFo["combo_token"];
    json::Json scanPayLoadJ;
    scanPayLoadJ.parse(scanPayloadR);
    scanPayLoadJ["raw"] = scanRawJ;
    scanPayLoadJ["ext"] = scanExtJ;
    scanResultJ["payload"] = scanPayLoadJ;
    scanResultJ["ts"] = (int)GetUnixTimeStampSeconds();
    scanResultJ["ticket"] = m_ticket;
    std::string postBody = scanResultJ.str();
    postBody = makeSign(postBody);
    json::Json postBodyJ;
    postBodyJ.parse(postBody);
    std::string a1 = postBodyJ["payload"]["ext"].str();
    a1 = replaceQuotes(a1);
    postBodyJ["payload"]["ext"] = a1;
    std::string a2 = postBodyJ["payload"]["raw"].str();
    a2 = replaceQuotes(a2);
    postBodyJ["payload"]["raw"] = a2;
    postBody = postBodyJ.str();
#ifdef _DEBUG
    std::cout << postBody << std::endl;
#endif // _DEBUG
    std::string response;
    PostRequest(response, mhy_bh3_qrcode_confirm, postBody);
    postBodyJ.parse(response);
    if ((int)postBodyJ["retcode"] == 0)
    {
        return ScanRet::SUCCESS;
    }
    else
    {
        return ScanRet::FAILURE_2;
    }
    return ScanRet::FAILURE_2;
}

void Mihoyosdk::setUserName(const std::string& name)
{
    json::Json setName;
    setName.parse(scanRawR);
    setName["asterisk_name"] = name;
    scanRawR = setName.str();
    setName.clear();
}

std::string Mihoyosdk::makeSign(const std::string data)
{
    std::string sign;
    std::string data2;
    json::Json p;
    p.parse(data);
    std::map<std::string, std::string> m = p.objToMap();
    for (auto& it : m)
    {
        if (it.first == "sign")
            continue;
        if (it.first == "data") //需要优化
        {
            if (it.second.front() == '\"')
            {
                it.second.erase(0, 1);
            }
            if (it.second.back() == '\"')
            {
                it.second.pop_back();
            }
        }
        if (it.first == "device")
        {
            if (it.second.front() == '\"')
            {
                it.second.erase(0, 1);
            }
            if (it.second.back() == '\"')
            {
                it.second.pop_back();
            }
        }
        data2 += it.first + "=" + it.second + "&";
    }
    data2.erase(data2.length() - 1);
#ifdef _DEBUG
    std::cout << "makeSign = " << data2 << std::endl;
#endif // DEBUG
    sign = bh3Sign(data2);
    p["sign"] = sign;
    sign = p.str();
    p.clear();
    return sign;
}

std::string Mihoyosdk::bh3Sign(std::string data)
{
    const std::string key = "0ebc517adb1b62c6b408df153331f9aa";
    data.erase(std::remove(data.begin(), data.end(), '\\'), data.end());
    std::string sign = HmacSha256(data, key);
#ifdef _DEBUG
    std::cout << "Hmac_Sha256签名完成" << std::endl;
#endif // DEBUG
    return sign;
}