#include "main.h"

v2api::v2api()
{
}

std::string v2api::Initialize()
{
    int realRoomID = GetRealRoomID();
    while (realRoomID == -1)
    {
        realRoomID = GetRealRoomID();
    }
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
        {"ts" , "1676498963"}
    };
    std::string playurl = GetHttpStream(V2API,params);
	playurl = utils.string_To_UTF8(playurl);
	return playurl;
}

int v2api::GetRealRoomID()
{
	std::cout << "请输入BiliBili直播间房间号：";
	std::string roomID;
	std::cin >> roomID;
	std::map<std::string, std::string> params = { {"id", roomID} };
	std::string addres = utils.Url(address, params);
	std::string result;
	utils.GetRequest(result,addres);
	result = utils.UTF8_To_string(result);
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
		std::cout << "直播间不存在" << std::endl;
		a.clear();
		return -1;
	}
	if (code == 0)
	{
		int liveStatus = a["data"]["live_status"];
		if (liveStatus != 1)
		{
			std::cout << "直播间未开播" << std::endl;
			a.clear();
			return -1;
		}
		else
		{
			int room_id = a["data"]["room_id"];
			a.clear();
			return room_id;
		}
	}
	std::cout << "未知错误" << std::endl;
	return -1;
}

std::string v2api::GetHttpStream(std::string api,std::map<std::string, std::string> param)
{
	std::string address = utils.Url(api, param);
	
	std::string str;
	utils.GetRequest(str,address);

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