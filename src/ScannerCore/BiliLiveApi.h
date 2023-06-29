#pragma once
#include <string>
#include"HttpClient.h"
#include"Json.h"

class v2api:public HttpClient
{
public:
	v2api();
	//获取直播流地址
	std::string GetAddress(int realRoomID);
	//获取真实直播间ID
	int GetRealRoomID(std::string roomID);
private:
	std::string GetStreamUrl(std::string api, std::map<std::string, std::string> param);
	int HandlerLiveStatus(std::string string);
private:
	//弃用V1api
	//const std::string V1API = "https://api.live.bilibili.com/xlive/web-room/v1/playUrl/playUrl";
	//获取直播间信息
	//https ://api.live.bilibili.com/room/v1/Room/get_info?room_id=6&from=room
	const std::string V2API = "https://api.live.bilibili.com/xlive/web-room/v2/index/getRoomPlayInfo";
	const std::string address = "https://api.live.bilibili.com/room/v1/Room/room_init";
};