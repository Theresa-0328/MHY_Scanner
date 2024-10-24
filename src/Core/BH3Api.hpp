#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <map>

#include <Json.h>

#include "HttpClient.h"
#include "TimeStamp.hpp"
#include "UtilString.hpp"
#include "CryptoKit.h"

namespace BH3API
{
namespace BILI
{
constexpr std::string_view userinfoParam{ R"({"cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","client_timestamp":"1667057013442","sdk_type":"1","isRoot":"0","merchant_id":"590","dp":"1280 * 720",
"mac":"08:00 : 27 : 53 : DD : 12","uid":"437470182","support_abis":"x86, armeabi - v7a, armeabi","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29",
"operators":"5","fingerprint":"","model":"MuMu","udid":"XXA31CBAB6CBA63E432E087B58411A213BFB7","net":"5","app_id":"180","brand":"Android","oaid":"","game_id":"180","timestamp":"1667057013275","ver":"6.1.0","c":"1",
"version_code":"510","server_id":"378","version":"1","domain_switch_count":"0","pf_ver":"12","access_key":"","domain":"line1 - sdk - center - login - sh.biligame.net","original_domain":"","imei":"","sdk_log_type":"1",
"sdk_ver":"3.4.2","android_id":"84567e2dda72d1d4","channel_id":1})" };

constexpr std::string_view rsaParam{ R"({"operators":"5", "merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type": "1","sdk_log_type":"1","timestamp":"1613035485639","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3",
"old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180",
"version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035486888","channel_id":"1","uid":"","game_id":"180","ver":"6.1.0",
"model":"MuMu"})" };

constexpr std::string_view loginParam{ R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035508188","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","gt_user_id":"fac83ce4326d47e1ac277a4d552bd2af","seccode":"","version":"1",
"udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"",
"validate":"84ec07cff0d9c30acb9fe46b8745e8df","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180",
"pwd":"rxwA8J+GcVdqa3qlvXFppusRg4Ss83tH6HqxcciVsTdwxSpsoz2WuAFFGgQKWM1GtFovrLkpeMieEwOmQdzvDiLTtHeQNBOiqHDfJEKtLj7h1nvKZ1Op6vOgs6hxM6fPqFGQC2ncbAR5NNkESpSWeYTO4IT58ZIJcC0DdWQqh4=",
"version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035509437","channel_id":"1","uid":"",
"captcha_type":"1","game_id":"180","challenge":"efc825eaaef2405c954a91ad9faf29a2","user_id":"doo349","ver":"6.1.0","model":"MuMu"})" };

constexpr std::string_view captchaParam{ R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035486182","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d",
"platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net",
"app_id":"180","version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035487431","channel_id":"1","uid":"","game_id":"180",
"ver":"6.1.0","model":"MuMu"})" };

const static std::map<std::string, std::string> headers{
    { "User-Agent", "Mozilla/5.0 BSGameSDK" },
    { "Content-Type", "application/x-www-form-urlencoded" },
    { "Host", "line1-sdk-center-login-sh.biligame.net" }
};

inline std::string SetSign(std::map<std::string, std::string> data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        std::string key = it->first;
        std::string value = it->second;
        key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
        it->second = value;
    }

    data["timestamp"] = std::to_string(GetUnixTimeStampSeconds());
    data["client_timestamp"] = std::to_string(GetUnixTimeStampSeconds());
    std::string sign;
    std::string data2;
    for (std::pair<std::string, std::string> c : data)
    {
        if (c.first == "pwd")
        {
            std::string pwd = urlEncode(c.second);
            data2 += c.first + "=" + pwd + "&";
        }
        data2 += c.first + "=" + c.second + "&";
    }
    for (std::pair<std::string, std::string> c : data)
    {
        sign += c.second;
    }
    sign += "dbf8f1b4496f430b8a3c0f436a35b931";
    return std::format("{}{}{}", data2, "sign=", Md5(sign));
}

inline auto GetUserInfo(const std::string& uid, const std::string& accessKey)
{
    json::Json userinfoParamj;
    userinfoParamj.parse(userinfoParam.data());
    userinfoParamj["uid"] = uid;
    userinfoParamj["access_key"] = accessKey;
    std::map<std::string, std::string> m = userinfoParamj.objToMap();
    for (auto& it : m)
    {
        std::string& str{ it.second };
        it.second = [&str]() -> std::string {
            std::string result;
            char* p{ &str[0] };
            while (*p != '\0')
            {
                if (*p != '"')
                {
                    result += *p;
                }
                p++;
            }
            return result;
        }();
    }
    std::string s{ SetSign(m) };
    std::string t;
    HttpClient h{};
    h.PostRequest(t, game_bili_userinfo, s, headers);
#ifdef _DEBUG
    std::cout << "BiliBili用户信息：" << t << std::endl;
#endif // _DEBUG
    struct
    {
        int code{};
        std::string uname{};
    } result;
    json::Json j{};
    j.parse(t);
    result.code = j["code"];
    if (result.code != 0)
    {
        return result;
    }
    result.uname = j["uname"];
    return result;
}

inline auto LoginByPassWord(
    const std::string_view biliAccount,
    const std::string_view biliPwd,
    const std::string_view gt_user = "",
    const std::string_view challenge = "",
    const std::string_view validate = "")
{
    json::Json data;
    data.parse(rsaParam.data());
    std::map<std::string, std::string> dataM = data.objToMap();
    std::string p1 = SetSign(dataM);
    std::string re;
    HttpClient h;
    h.PostRequest(re, game_bili_rsa, p1, headers);
#ifdef _DEBUG
    std::cout << re << std::endl;
#endif // _DEBUG
    data.parse(loginParam.data());
    json::Json re1J;
    re1J.parse(re.data());
    std::string publicKey = re1J["rsa_key"];
    FormatRsaPublicKey(publicKey);
    data["access_key"] = "";
    data["gt_user_id"] = gt_user.data();
    data["uid"] = "";
    data["challenge"] = challenge.data();
    data["user_id"] = biliAccount.data();
    data["validate"] = validate.data();
    if (!validate.empty())
    {
        data["seccode"] = validate.data() + std::string{ "|jordan" };
    }
    std::string hash1 = re1J["hash"];
    std::string rekit = rsaEncrypt(hash1 + biliPwd.data(), publicKey);
    data["pwd"] = rekit;
#ifdef _DEBUG
    std::cout << data.str() << std::endl;
#endif // _DEBUG
    std::map<std::string, std::string> dataR = data.objToMap();
    std::string p2 = SetSign(dataR);
    re.clear();
    data.clear();
    re1J.clear();
    h.PostRequest(re, game_bili_login, p2, headers);
    json::Json j1;
    j1.parse(re);
    struct
    {
        int code{};
        std::string message{};
        std::string uid{};
        std::string access_key{};
        std::string uname{};
    } result;
    result.code = j1["code"];
    if (result.code == 20000 || result.code != 0)
    {
        try
        {
            result.message = j1["message"];
        }
        catch (const std::exception&)
        {
            result.message = "意外登录错误";
        }
        return result;
    }
    result.uid = std::to_string((int)j1["uid"]);
    result.access_key = j1["access_key"];
    auto UserInforesult{ GetUserInfo(result.uid, result.access_key) };
    result.uname = UserInforesult.uname;
    return result;
}

inline auto CaptchaCaptcha()
{
    json::Json data;
    data.parse(captchaParam.data());
    std::map<std::string, std::string> info = data.objToMap();
    std::string data1 = SetSign(info);
    std::string data2;
    HttpClient h;
    h.PostRequest(data2, game_bili_start_captcha, data1, headers);
    json::Json captchaJ;
    captchaJ.parse(data2);
    struct
    {
        std::string gt{};
        std::string challenge{};
        std::string gt_user_id{};
    } result;
    result.gt = captchaJ["gt"];
    result.challenge = captchaJ["challenge"];
    result.gt_user_id = captchaJ["gt_user_id"];
    return result;
}

}
}