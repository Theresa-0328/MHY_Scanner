#pragma once

#include <string>

#include "HttpClient.h"

class LiveStreamStatus
{
public:
	enum Status
	{
		Normal = 0,
		Absent = 1,
		NotLive = 2,
		Error = 3
	};
};

class LiveBili :public HttpClient
{
public:
	LiveBili(const std::string& roomID);
	LiveStreamStatus::Status GetLiveStreamStatus();
	std::string GetLiveStreamLink();
private:
	//获取B站直播流地址
	std::string GetLinkByRealRoomID(const std::string& realRoomID);
	std::string GetStreamUrl(const std::string& url, const std::map<std::string, std::string>& param);
	std::string m_roomID;
	std::string m_realRoomID;
	//获取直播间信息接口
	//https://api.live.bilibili.com/room/v1/Room/get_info?room_id=6&from=room
};

class LiveHuya :public HttpClient
{
public:
	LiveHuya(const std::string& roomID);
	LiveStreamStatus::Status GetLiveStreamStatus();
	std::string GetLiveStreamLink()const;
	~LiveHuya();
private:
	std::string m_roomID;
	std::string m_flvUrl;
	std::string GetanonymousUid();
	std::string process_anticode(const std::string& anticode, const std::string& uid, const std::string& streamname);
	std::string getUUID();
};

class LiveDouyin :public HttpClient
{
public:
	LiveDouyin(const std::string& roomID);
	LiveStreamStatus::Status GetLiveStreamStatus();
	std::string GetLiveStreamLink();
	~LiveDouyin();
private:
	std::string m_roomID;
};