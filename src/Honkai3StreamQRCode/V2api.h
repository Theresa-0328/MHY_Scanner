#pragma once
#include <string>
#include <map>
#include"HttpClient.h"
#include"Json.h"

class v2api:public HttpClient
{
public:
	v2api();
	std::string Initialize();
	std::string GetHttpStream(std::string api, std::map<std::string, std::string> param);
private:
	int GetRealRoomID();
	int HandlerLiveStatus(std::string string);
private:
	//const std::string V1API = "https://api.live.bilibili.com/xlive/web-room/v1/playUrl/playUrl";
	const std::string V2API = "https://api.live.bilibili.com/xlive/web-room/v2/index/getRoomPlayInfo";
	const std::string address = "https://api.live.bilibili.com/room/v1/Room/room_init";
	std::map<std::string, std::string> headers =
	{
		{"User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64)AppleWebKit/537.36(KHTML,likeGecko)Chrome/110.0.0.0Safari/537.36 Edg/110.0.1587.41"},
		{"Referer","https://live.bilibili.com"}
	};
};