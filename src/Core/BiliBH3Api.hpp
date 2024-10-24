#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <map>

namespace BH3API
{
namespace BILI
{
constexpr std::string_view userinfoParam = R"({"cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","client_timestamp":"1667057013442","sdk_type":"1","isRoot":"0","merchant_id":"590","dp":"1280 * 720","mac":"08:00 : 27 : 53 : DD : 12","uid":"437470182","support_abis":"x86, armeabi - v7a, armeabi","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","operators":"5","fingerprint":"","model":"MuMu","udid":"XXA31CBAB6CBA63E432E087B58411A213BFB7","net":"5","app_id":"180","brand":"Android","oaid":"","game_id":"180","timestamp":"1667057013275","ver":"6.1.0","c":"1","version_code":"510","server_id":"378","version":"1","domain_switch_count":"0","pf_ver":"12","access_key":"","domain":"line1 - sdk - center - login - sh.biligame.net","original_domain":"","imei":"","sdk_log_type":"1","sdk_ver":"3.4.2","android_id":"84567e2dda72d1d4","channel_id":1})";

constexpr std::string_view rsaParam = R"({"operators":"5", "merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type": "1","sdk_log_type":"1","timestamp":"1613035485639","support_abis":"x86,armeabi-v7a,armeabi","access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180","version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035486888","channel_id":"1","uid":"","game_id":"180","ver":"6.1.0","model":"MuMu"})";

constexpr std::string_view loginParam = R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035508188","support_abis":"x86,armeabi-v7a,armeabi","access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","gt_user_id":"fac83ce4326d47e1ac277a4d552bd2af","seccode":"","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","validate":"84ec07cff0d9c30acb9fe46b8745e8df","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180","pwd":"rxwA8J+GcVdqa3qlvXFppusRg4Ss83tH6HqxcciVsTdwxSpsoz2WuAFFGgQKWM1GtFovrLkpeMieEwOmQdzvDiLTtHeQNBOiqHDfJEKtLj7h1nvKZ1Op6vOgs6hxM6fPqFGQC2ncbAR5NNkESpSWeYTO4IT58ZIJcC0DdWQqh4=","version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035509437","channel_id":"1","uid":"","captcha_type":"1","game_id":"180","challenge":"efc825eaaef2405c954a91ad9faf29a2","user_id":"doo349","ver":"6.1.0","model":"MuMu"})";

constexpr std::string_view captchaParam = R"({"operators":"5","merchant_id":"590","isRoot":"0","domain_switch_count":"0","sdk_type":"1","sdk_log_type":"1","timestamp":"1613035486182","support_abis":"x86,armeabi-v7a,armeabi","access_key":"","sdk_ver":"3.4.2","oaid":"","dp":"1280*720","original_domain":"","imei":"227656364311444","version":"1","udid":"KREhESMUIhUjFnJKNko2TDQFYlZkB3cdeQ==","apk_sign":"4502a02a00395dec05a4134ad593224d","platform_type":"3","old_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","android_id":"84567e2dda72d1d4","fingerprint":"","mac":"08:00:27:53:DD:12","server_id":"378","domain":"line1-sdk-center-login-sh.biligame.net","app_id":"180","version_code":"510","net":"4","pf_ver":"12","cur_buvid":"XZA2FA4AC240F665E2F27F603ABF98C615C29","c":"1","brand":"Android","client_timestamp":"1613035487431","channel_id":"1","uid":"","game_id":"180","ver":"6.1.0","model":"MuMu"})";

const static std::map<std::string, std::string> headers{
    { "User-Agent", "Mozilla/5.0 BSGameSDK" },
    { "Content-Type", "application/x-www-form-urlencoded" },
    { "Host", "line1-sdk-center-login-sh.biligame.net" }
};

inline void LoginByPassWord(const std::string_view account, const std::string_view pwd)
{
}

inline void LoginByPassWord(const std::string_view biliAccount, const std::string_view biliPwd, const std::string_view challenge = "", const std::string_view gt_user = "", const std::string_view validate = "")
{
}

}
}