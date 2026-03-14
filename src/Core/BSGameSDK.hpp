#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <map>

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "TimeStamp.hpp"
#include "UtilString.hpp"
#include "CryptoKit.h"
#include "ApiDefs.hpp"

namespace BSGameSDK
{
namespace BH3
{
namespace detail
{
constinit const std::string_view userinfoParam{ R"({"cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","client_timestamp":"1667057013442","sdk_type":"1","isRoot":"0","merchant_id":"590","dp":"1280 * 720",
"mac":"08:00:27:53:DD:12","uid":"437470182","support_abis":"x86,armeabi-v7a,armeabi","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29",
"operators":"5","fingerprint":"","model":"MuMu","udid":"XXA31CBAB6CBA63E432E087B58411A213BFB7","net":"5","app_id":"180","brand":"Android","oaid":"","game_id":"180","timestamp":"1667057013275","ver":"6.1.0","c":"1",
"version_code":"510","server_id":"378","version":"1","domain_switch_count":"0","pf_ver":"12","access_key":"","domain":"line1-sdk-center-login-sh.biligame.net","original_domain":"","imei":"","sdk_log_type":"1",
"sdk_ver":"3.4.2","android_id":"84567e2dda72d1d4","channel_id":1})" };

constinit const std::string_view rsaParam{ R"({"operators":"5", "merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type": "1","sdk_log_type":"1","timestamp":"1613035485639","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3",
"old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180",
"version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035486888","channel_id":"1","uid":"","game_id":"180","ver":"6.1.0",
"model":"MuMu"})" };

constinit const std::string_view loginParam{ R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035508188","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","gt_user_id":"fac83ce4326d47e1ac277a4d552bd2af","seccode":"","version":"1",
"udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"",
"validate":"84ec07cff0d9c30acb9fe46b8745e8df","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180",
"pwd":"rxwA8J+GcVdqa3qlvXFppusRg4Ss83tH6HqxcciVsTdwxSpsoz2WuAFFGgQKWM1GtFovrLkpeMieEwOmQdzvDiLTtHeQNBOiqHDfJEKtLj7h1nvKZ1Op6vOgs6hxM6fPqFGQC2ncbAR5NNkESpSWeYTO4IT58ZIJcC0DdWQqh4=",
"version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035509437","channel_id":"1","uid":"",
"captcha_type":"1","game_id":"180","challenge":"efc825eaaef2405c954a91ad9faf29a2","user_id":"doo349","ver":"6.1.0","model":"MuMu"})" };

constinit const std::string_view captchaParam{ R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035486182","support_abis":"x86,armeabi-v7a,armeabi",
"access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d",
"platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net",
"app_id":"180","version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035487431","channel_id":"1","uid":"","game_id":"180",
"ver":"6.1.0","model":"MuMu"})" };

static const cpr::Header headers{
    { "User-Agent", "Mozilla/5.0 BSGameSDK" },
    { "Content-Type", "application/x-www-form-urlencoded" },
    { "Host", "line1-sdk-center-login-sh.biligame.net" }
};

inline std::string SetSign(nlohmann::json data)
{
    data["timestamp"] = std::to_string(GetUnixTimeStampSeconds());
    data["client_timestamp"] = std::to_string(GetUnixTimeStampSeconds());

    std::string sign;
    std::string body;

    auto getValue = [](const nlohmann::json& v) -> std::string {
        return v.is_string() ? v.get<std::string>() : v.dump();
    };

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        std::string key = it.key();
        std::string value = getValue(it.value());

        if (key == "pwd")
        {
            body += key + "=" + urlEncode(value) + "&";
        }
        else
        {
            body += key + "=" + value + "&";
        }
    }

    for (auto it = data.begin(); it != data.end(); ++it)
    {
        sign += getValue(it.value());
    }

    sign += "dbf8f1b4496f430b8a3c0f436a35b931";
    return body + "sign=" + Md5(sign);
}

inline std::string GetEncryptedPwd(const std::string_view pwd)
{
    nlohmann::json data = nlohmann::json::parse(detail::rsaParam);
    std::string body = detail::SetSign(data);

    auto rsaResponse = cpr::Post(
        cpr::Url{ api::game::bili::rsa },
        cpr::Body{ body },
        cpr::Header{ detail::headers });

#ifdef _DEBUG
    std::cout << rsaResponse.text << std::endl;
#endif

    nlohmann::json rsaInfo = nlohmann::json::parse(rsaResponse.text);
    std::string publicKey = rsaInfo["rsa_key"];
    std::string hash = rsaInfo["hash"];
    return rsaEncrypt(hash + std::string{ pwd }, publicKey);
}
}

inline auto GetUserInfo(const std::string& uid, const std::string& accessKey)
{
    nlohmann::json data = nlohmann::json::parse(detail::userinfoParam);
    data["uid"] = uid;
    data["access_key"] = accessKey;

    std::string body{ detail::SetSign(data) };

    auto response = cpr::Post(
        cpr::Url{ api::game::bili::userinfo },
        cpr::Body{ body },
        cpr::Header{ detail::headers });
#ifdef _DEBUG
    std::cout << "BiliBili UserInfo ：" << response.text << std::endl;
#endif
    struct
    {
        int code{};
        std::string uname{};
    } result;
    nlohmann::json info = nlohmann::json::parse(response.text);
    result.code = info["code"];
    if (result.code != 0)
    {
        return result;
    }
    result.uname = info["uname"];
    return result;
}

inline auto LoginByPassWord(
    const std::string_view biliAccount,
    const std::string_view biliPwd,
    const std::string_view gt_user = "",
    const std::string_view challenge = "",
    const std::string_view validate = "")
{
    nlohmann::json data = nlohmann::json::parse(detail::loginParam);
    data["access_key"] = "";
    data["gt_user_id"] = gt_user;
    data["uid"] = "";
    data["challenge"] = challenge;
    data["user_id"] = biliAccount;
    data["validate"] = validate;
    if (!validate.empty())
    {
        data["seccode"] = std::string{ validate } + "|jordan";
    }
    data["pwd"] = detail::GetEncryptedPwd(biliPwd);

#ifdef _DEBUG
    std::cout << data.dump() << std::endl;
#endif

    std::string body = detail::SetSign(data);
    auto loginResponse = cpr::Post(
        cpr::Url{ api::game::bili::login },
        cpr::Body{ body },
        detail::headers);

    struct
    {
        int code{};
        std::string message{};
        std::string uid{};
        std::string access_key{};
        std::string uname{};
    } result;

    nlohmann::json loginInfo = nlohmann::json::parse(loginResponse.text);
    result.code = loginInfo["code"];
    if (result.code == 20000 || result.code != 0)
    {
        try
        {
            result.message = loginInfo["message"];
        }
        catch (const std::exception&)
        {
            result.message = "意外登录错误";
        }
        return result;
    }

    result.uid = std::to_string(loginInfo["uid"].get<int>());
    result.access_key = loginInfo["access_key"];
    auto userInfoResult{ GetUserInfo(result.uid, result.access_key) };
    result.uname = userInfoResult.uname;
    return result;
}

inline GeetestData CaptchaCaptcha()
{
    nlohmann::json data = nlohmann::json::parse(detail::captchaParam);
    std::string body = detail::SetSign(data);
    auto response = cpr::Post(
        cpr::Url{ api::game::bili::start_captcha },
        cpr::Body{ body },
        detail::headers);
    nlohmann::json captcha = nlohmann::json::parse(response.text);
    GeetestData result{};
    result.gt = captcha["gt"];
    result.challenge = captcha["challenge"];
    result.session_id = captcha["gt_user_id"];
    result.GeeTestType = ServerType::BH3_BiliBili;
    return result;
}

}
}