#pragma once
#include "HttpClient.h"
#include <unordered_map>
class OfficialApi :public HttpClient
{
public:
	std::string getRole();
	std::string getUserName(std::string uid);
	void cookieParser(const std::string& cookieString);
	int scanRequest(std::string ticket, std::string uid, std::string token);
	std::string getGameToken();
	std::string getUid();
	int gameType;
private:
	std::string getMultiTokenByLoginTicket();
	int confirmRequest(std::string UUID, std::string ticket, std::string uid, std::string token);
	std::string generateUUID();
	std::string getDS();
	std::string data;
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