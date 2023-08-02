#pragma once
#include <unordered_map>
#include "HttpClient.h"

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

class OfficialApi :public HttpClient
{
public:
	//获取账号上所有的角色,当前不可用！
	[[deprecated("This function is temporarily deprecated!")]]
	std::string getRole();
	int getMultiTokenByLoginTicket(std::string& data);
	std::string getUserName(std::string uid);
	int cookieParser(const std::string& cookieString);
	//扫码请求
	int scanRequest(const std::string& ticket, const std::string& uid, const std::string& token, const std::string& uuid);
	int getGameToken(const std::string& stoken, const std::string& uid, std::string& gameToken);
	void setGameType(const GameType::Type& gameType);
	std::string getUid()const;
	std::string generateUUID();
	std::string getDS2();
private:
	GameType::Type m_gameType = GameType::Type::UNKNOW;
	int confirmRequest(const std::string& UUID, const std::string& ticket,
		const std::string& uid, const std::string& token, const std::string& url);
	std::unordered_map<std::string, std::string> cookieMap;
	const std::string salt = "PVeGWIZACpxXZ1ibMVJPi9inCY4Nd4y2";
	const std::string app_version = "2.38.1";
	std::map<std::string, std::string> headers =
	{
		{"x-rpc-client_type", "2"},
		{"x-rpc-app_version", app_version},
		{"x-rpc-sys_version", "7.1.2"},
		{"x-rpc-channel", "miyousheluodi"},
		{"x-rpc-device_id", "d9951154-6eea-35e8-9e46-20c53f440ac7"},
		{"x-rpc-device_fp", "38d7ed301ed62"},
		{"x-rpc-device_name", "HUAWEI LIO-AN00"},
		{"x-rpc-device_model", "LIO-AN00"},
		{"Referer", "https://app.mihoyo.com"},
		{"Content-Type", "application/json; charset=UTF-8"},
		{"Host", "api-sdk.mihoyo.com"},
		{"Connection", "Keep-Alive"},
		{"User-Agent", "okhttp/4.9.3"}
	};
};