#include "LiveStreamLink.h"

#include <format>

#include <Json.h>

LiveBili::LiveBili(const std::string& roomID) :m_roomID(roomID)
{

}

LiveStreamStatus::Status LiveBili::GetLiveStreamStatus()
{
	std::map<std::string, std::string> params = { {"id", m_roomID} };
	const std::string& addres = Url(live_bili_room_init, params);
	std::string result;
	GetRequest(result, addres);
	json::Json data;
	data.parse(result);
	int code = data["code"];
	if (code == 60004)
	{
		//直播间不存在
		data.clear();
		return LiveStreamStatus::Status::Absent;
	}
	if (code == 0)
	{
		int liveStatus = data["data"]["live_status"];
		if (liveStatus != 1)
		{
			//直播间未开播
			data.clear();
			return LiveStreamStatus::Status::NotLive;
		}
		else
		{
			// 设置真实roomid
			m_realRoomID = data["data"]["room_id"].str();
			data.clear();
			return LiveStreamStatus::Status::Normal;
		}
	}
	//其他错误
	return LiveStreamStatus::Status::Error;
}

std::string LiveBili::GetLiveStreamLink()
{
	return GetLinkByRealRoomID(m_realRoomID);
}

std::string LiveBili::GetLinkByRealRoomID(const std::string& realRoomID)
{
	std::map<std::string, std::string> params =
	{
		//appkey:iVGUTjsxvpLeuDCf
		//build:6215200
		//c_locale:zh_CN
		{"codec" , "0"},
		//device:web
		//device_name:VTR-AL00
		//dolby:1
		{"format" , "0,2"},
		//free_type:0
		//http:1
		//mask:0
		//mobi_app:web
		//network:wifi
		//no_playurl:0
		{"only_audio" , "0"},
		{"only_video" , "0"},
		//TODO platform 会影响下载时使用的referer
		//{"platform", "h5" },
		//play_type:0
		{"protocol" , "0,1"},
		//TODO 考虑存在低清晰度时使用低清晰度？
		{"qn" , "10000"},
		{"room_id" , realRoomID},
		//s_locale:zh_CN
		//statistics:{\"appId\":1,\"platform\":3,\"version\":\"6.21.5\",\"abtest\":\"\"}
	};
	return GetStreamUrl(live_bili_v2_PlayInfo, params);
}

std::string LiveBili::GetStreamUrl(const std::string& url, std::map<std::string, std::string> param)
{
	std::string address = Url(url, param);

	std::string str;
	GetRequest(str, address);

	json::Json j;
	j.parse(str);
	const std::string& base_url = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["base_url"];
	const std::string& extra = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["url_info"][0]["extra"];
	const std::string& host = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["url_info"][0]["host"];
	std::string stream_url = std::format("{}{}{}", host, base_url, extra);

	//FIXME
	size_t pos = stream_url.find("0026");
	while (pos != std::string::npos)
	{
		stream_url.replace(pos, 4, "&");
		pos = stream_url.find("0026");
	}
#ifdef _DEBUG
	std::cout << stream_url << std::endl;
#endif // _DEBUG
	return stream_url;
}

LiveHuya::LiveHuya(const std::string& roomID)
{

}

LiveHuya::~LiveHuya()
{

}

LiveDoyin::LiveDoyin(const std::string& roomID) :m_roomID(roomID)
{

}

LiveStreamStatus::Status LiveDoyin::GetLiveStreamStatus()
{
	return LiveStreamStatus::Status();
}

std::string LiveDoyin::GetLiveStreamLink()
{
	std::string ret;
	std::map <std::string, std::string> header =
	{
		{"User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36 Edg/115.0.1901.203"}
	};
	GetRequest(ret, std::format("{}/{}", live_douyin, m_roomID), header);
	return std::string();
}

LiveDoyin::~LiveDoyin()
{

}