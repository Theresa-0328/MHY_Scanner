#pragma once

class ServerType
{
public:
	enum Type
	{
		UNKNOW = 0,
		Official = 1,
		BH3_BiliBili = 2
	};
};

class GameType
{
public:
	enum Type
	{
		UNKNOW = 0,
		Honkai3 = 1,
		Genshin = 4,
		StarRail = 8
	};
};

class ScanRet
{
public:
	enum Type
	{
		UNKNOW = 0,
		SUCCESS = 1,
		FAILURE_1 = 3,
		FAILURE_2 = 4,
		LIVESTOP = 5,
		STREAMERROR = 6
	};
};

constexpr const char* game_bili_userinfo = "https://line1-sdk-center-login-sh.biligame.net/api/client/user.info";
constexpr const char* game_bili_start_captcha = "https://line1-sdk-center-login-sh.biligame.net/api/client/start_captcha";
constexpr const char* game_bili_login = "https://line1-sdk-center-login-sh.biligame.net/api/client/login";
constexpr const char* game_bili_rsa = "https://line1-sdk-center-login-sh.biligame.net/api/client/rsa";

constexpr const char* live_bili_room_init = "https://api.live.bilibili.com/room/v1/Room/room_init";
//constexpr const char* live_bili_v1_playUrl = "https://api.live.bilibili.com/xlive/web-room/v1/playUrl/playUrl";//弃用
constexpr const char* live_bili_v2_PlayInfo = "https://api.live.bilibili.com/xlive/web-room/v2/index/getRoomPlayInfo";

constexpr const char* live_douyin = "https://live.douyin.com";

constexpr const char* live_huya = "https://m.huya.com/";

constexpr const char* mhy_bh3_v2_login = "https://api-sdk.mihoyo.com/bh3_cn/combo/granter/login/v2/login";
constexpr const char* mhy_bh3_version = "https://bh3-launcher-static.mihoyo.com/bh3_cn/mdk/launcher/api/resource?launcher_id=4";
constexpr const char* mhy_bh3_qrcode_scan = "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/scan";
constexpr const char* mhy_bh3_qrcode_confirm = "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/confirm";

constexpr const char* mhy_hk4e_qrcode_scan = "https://api-sdk.mihoyo.com/hk4e_cn/combo/panda/qrcode/scan";
constexpr const char* mhy_hk4e_qrcode_confirm = "https://api-sdk.mihoyo.com/hk4e_cn/combo/panda/qrcode/confirm";

constexpr const char* mhy_hkrpg_qrcode_scan = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
constexpr const char* mhy_hkrpg_qrcode_confirm = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/confirm";

constexpr const char* mhy_takumi_multi_token_by_login_ticket = "https://api-takumi.mihoyo.com/auth/api/getMultiTokenByLoginTicket";

constexpr const char* mhy_mys_uesrinfo = "https://bbs-api.miyoushe.com/user/api/getUserFullInfo";