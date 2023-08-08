#pragma once
#include <string>
#include"Json.h"
#include"HttpClient.h"

class v2api :public HttpClient
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
	//获取直播间信息接口
	//https://api.live.bilibili.com/room/v1/Room/get_info?room_id=6&from=room
};