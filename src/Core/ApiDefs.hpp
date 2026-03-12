#pragma once

#include <string_view>

#include "compile_string.hpp"

enum class ServerType : uint8_t
{
    UNKNOW [[deprecated]] = 0,
    Official = 1,
    BH3_BiliBili = 2
};

enum class GameType
{
    UNKNOW = 0,
    Honkai3 = 1,
    TearsOfThemis = 2,
    Genshin = 4,
    PlatformApp = 5,
    Honkai2 = 7,
    HonkaiStarRail = 8,
    CloudGame = 9,
    _3NNN = 10,
    PJSH = 11,
    ZenlessZoneZero = 12,
    HYG = 13,
    Honkai3_BiliBili = 10000
};

enum class ScanRet
{
    UNKNOW = 0,
    SUCCESS = 1,
    FAILURE_1 = 3,
    FAILURE_2 = 4,
    LIVESTOP = 5,
    STREAMERROR = 6
};

enum class LoginQRCodeState
{
    Init = 0,
    Scanned = 1,
    Confirmed = 2,
    Expired = 3
};

struct GeetestData
{
    std::string action_type{};
    std::string session_id{};
    int mmt_type{};
    std::string gt{};
    std::string challenge{};
    ServerType GeeTestType{};
};

constinit const std::string_view mihoyobbs_version{ "2.75.2" };
constinit const std::string_view mihoyobbs_salt{ "oqrJbPCoFhWhFBNDvVRuldbrbiVxyWsP" };
constinit const std::string_view mihoyobbs_salt_web{ "zZDfHqEcwTqvvKDmqRcHyqqurxGgLfBV" };

constinit const std::string_view mihoyobbs_salt_x4{ "xV8v4Qu54lUKrEYFZkJhB8cuOh9Asafs" };
constinit const std::string_view mihoyobbs_salt_x6{ "t0qEgfub6cvueAPgR5m9aQWWVciEer7v" };

namespace api::mhy
{

constexpr compile_string api_sdk{ "https://api-sdk.mihoyo.com" };

namespace bh3
{
constexpr auto base = api_sdk + compile_string{ "/bh3_cn" };
constexpr auto v2_login = base + compile_string{ "/combo/granter/login/v2/login" };
constexpr auto qrcode_scan = base + compile_string{ "/combo/panda/qrcode/scan" };
constexpr auto qrcode_confirm = base + compile_string{ "/combo/panda/qrcode/confirm" };

constexpr compile_string version{ "https://bh3-launcher-static.mihoyo.com/bh3_cn/mdk/launcher/api/resource?launcher_id=4" };
}

namespace hk4e
{
constexpr auto base = api_sdk + compile_string{ "/hk4e_cn" };
constexpr auto qrcode_scan = base + compile_string{ "/combo/panda/qrcode/scan" };
constexpr auto qrcode_confirm = base + compile_string{ "/combo/panda/qrcode/confirm" };

constexpr compile_string sdk_base{ "https://hk4e-sdk.mihoyo.com/hk4e_cn" };
constexpr auto qrcode_fetch = sdk_base + compile_string{ "/combo/panda/qrcode/fetch" };
constexpr auto qrcode_query = sdk_base + compile_string{ "/combo/panda/qrcode/query" };
}

namespace hkrpg
{
constexpr auto base = api_sdk + compile_string{ "/hkrpg_cn" };
constexpr auto qrcode_scan = base + compile_string{ "/combo/panda/qrcode/scan" };
constexpr auto qrcode_confirm = base + compile_string{ "/combo/panda/qrcode/confirm" };
}

namespace nap
{
constexpr auto base = api_sdk + compile_string{ "/nap_cn" };
constexpr auto qrcode_scan = base + compile_string{ "/combo/panda/qrcode/scan" };
constexpr auto qrcode_confirm = base + compile_string{ "/combo/panda/qrcode/confirm" };
}

namespace takumi
{
constexpr compile_string base{ "https://api-takumi.mihoyo.com" };
constexpr auto multi_token = base + compile_string{ "/auth/api/getMultiTokenByLoginTicket" };
constexpr auto game_token = base + compile_string{ "/auth/api/getGameToken" };
constexpr auto game_token_stoken = base + compile_string{ "/account/ma-cn-session/app/getTokenByGameToken" };
}

namespace passport
{
constexpr compile_string base{ "https://passport-api.mihoyo.com" };
constexpr auto create_captcha = base + compile_string{ "/account/ma-cn-verifier/verifier/createLoginCaptcha" };
constexpr auto login_by_mobile_captcha = base + compile_string{ "/account/ma-cn-passport/app/loginByMobileCaptcha" };
}

namespace mys
{
constexpr compile_string base{ "https://bbs-api.miyoushe.com" };
constexpr auto userinfo = base + compile_string{ "/user/api/getUserFullInfo" };
}

}

namespace api::game
{

namespace bili
{
constexpr compile_string base{ "https://line1-sdk-center-login-sh.biligame.net" };
constexpr auto userinfo = base + compile_string{ "/api/client/user.info" };
constexpr auto start_captcha = base + compile_string{ "/api/client/start_captcha" };
constexpr auto login = base + compile_string{ "/api/client/login" };
constexpr auto rsa = base + compile_string{ "/api/client/rsa" };
}

}

namespace api::live
{

namespace bili
{
constexpr compile_string base{ "https://api.live.bilibili.com" };
constexpr auto room_init = base + compile_string{ "/room/v1/Room/room_init" };
// constexpr auto v1_play_url = base + compile_string{ "/xlive/web-room/v1/playUrl/playUrl" }; // 弃用
constexpr auto v2_play_info = base + compile_string{ "/xlive/web-room/v2/index/getRoomPlayInfo" };
}

namespace douyin
{
constexpr compile_string base{ "https://live.douyin.com" };
constexpr auto room = base + compile_string{ "/webcast/room/web/enter/?" };
}

}