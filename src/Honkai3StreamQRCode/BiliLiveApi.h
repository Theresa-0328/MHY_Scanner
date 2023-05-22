#pragma once
#include <string>
#include"HttpClient.h"
#include"Json.h"

class v2api:public HttpClient
{
public:
	v2api();
	std::string GetAddress(int realRoomID);
	int GetRealRoomID(std::string roomID);
private:
	std::string GetStreamUrl(std::string api, std::map<std::string, std::string> param);
	int HandlerLiveStatus(std::string string);
private:
	//const std::string V1API = "https://api.live.bilibili.com/xlive/web-room/v1/playUrl/playUrl";
	const std::string V2API = "https://api.live.bilibili.com/xlive/web-room/v2/index/getRoomPlayInfo";
	const std::string address = "https://api.live.bilibili.com/room/v1/Room/room_init";
};