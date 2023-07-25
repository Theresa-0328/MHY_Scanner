#include "LiveBili.h"

v2api::v2api()
{
}

std::string v2api::GetAddress(int realRoomID)
{
#ifdef _DEBUG
	std::cout << "realRoomID:" << realRoomID << std::endl;
#endif
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
		//platform:web
		//play_type:0
		{"protocol" , "0,1"},
		{"qn" , "10000"},
		{"room_id" , std::to_string(realRoomID)},
		//s_locale:zh_CN
		//statistics:{\"appId\":1,\"platform\":3,\"version\":\"6.21.5\",\"abtest\":\"\"}
	};
	//弃用v1api
	//const std::string v1API = "https://api.live.bilibili.com/xlive/web-room/v1/playUrl/playUrl";
	const std::string v2API = "https://api.live.bilibili.com/xlive/web-room/v2/index/getRoomPlayInfo";
	std::string playurl = GetStreamUrl(v2API, params);
	playurl = string_To_UTF8(playurl);
	return playurl;
}

int v2api::GetRealRoomID(std::string roomID)
{
	std::map<std::string, std::string> params = { {"id", roomID} };
	const std::string address = "https://api.live.bilibili.com/room/v1/Room/room_init";
	std::string addres = Url(address, params);
	std::string result;
	GetRequest(result, addres);
	result = UTF8_To_string(result);
	int realRoomID = HandlerLiveStatus(result);
	return realRoomID;
}


int v2api::HandlerLiveStatus(std::string string)
{
	json::Json a;
	a.parse(string);
	int code = a["code"];
	if (code == 60004)
	{
		//直播间不存在
		a.clear();
		return -1;
	}
	if (code == 0)
	{
		int liveStatus = a["data"]["live_status"];
		if (liveStatus != 1)
		{
			//直播间未开播
			a.clear();
			return -2;
		}
		else
		{
			int room_id = a["data"]["room_id"];
			a.clear();
			return room_id;
		}
	}
	//未知错误
	return -3;
}

std::string v2api::GetStreamUrl(std::string api, std::map<std::string, std::string> param)
{
	std::string address = Url(api, param);

	std::string str;
	GetRequest(str, address);

	json::Json j;
	j.parse(str);
	std::string baseUrl = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["base_url"];
	std::string extra = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["url_info"][0]["extra"];
	std::string host = j["data"]["playurl_info"]["playurl"]["stream"][0]["format"][0]["codec"][0]["url_info"][0]["host"];
	std::string streamUrl = host + baseUrl + extra;

	size_t pos = streamUrl.find("0026");
	while (pos != std::string::npos)
	{
		streamUrl.replace(pos, 4, "&");
		pos = streamUrl.find("0026");
	}

#ifdef _DEBUG
	std::cout << streamUrl << std::endl;
#endif // _DEBUG
	return streamUrl;
}
