#include "LiveStreamLink.h"

#include <format>
#include <fstream>
#include <regex>
#include <random>

#include <Json.h>
#include <Base64.hpp>

LiveBili::LiveBili(const std::string& roomID) :m_roomID(roomID)
{

}

LiveStreamStatus::Status LiveBili::GetLiveStreamStatus()
{
	std::string result;
	GetRequest(result, std::format("{}?id={}", live_bili_room_init, m_roomID));
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
			// 正在直播，设置真实roomid
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
	const std::map<std::string, std::string>& params =
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

std::string LiveBili::GetStreamUrl(const std::string& url, const std::map<std::string, std::string>& param)
{
	std::string str;
	GetRequest(str, std::format("{}?{}", url, MapToQueryString(param)));

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
	return stream_url;
}

LiveHuya::LiveHuya(const std::string& roomID) :m_roomID(roomID)
{

}

LiveStreamStatus::Status LiveHuya::GetLiveStreamStatus()
{
	std::string ret;
	const std::map <std::string, std::string>& header =
	{
		{"Content-Type","text/html; charset=utf-8"},
		{"User-Agent","Mozilla/5.0 (Linux; Android 5.0; SM-G900P Build/LRX21T) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/75.0.3770.100 Mobile Safari/537.36"},
		{"Accept-Encoding",""}
	};
	GetRequest(ret, std::format("{}/{}", live_huya, m_roomID), header);

	std::smatch matches;
	std::string s1;
	if (std::regex_search(ret, matches, std::regex("<script> window\\.HNF_GLOBAL_INIT = (.*?) </script>")))
	{
		if (matches.size() > 1)
		{
			s1 = matches[1];
		}
	}
	json::Json roomInfo;
	roomInfo.parse(s1);
	if (s1 == R"({"exceptionType":0})")
	{
		return LiveStreamStatus::Absent;
	}
	if ((int)roomInfo["roomInfo"]["eLiveStatus"] == 2)
	{
		json::Json s;
		s.parse(roomInfo["roomInfo"]["tLiveInfo"]["tLiveStreamInfo"]["vStreamInfo"]["value"][0].str());
		m_flvUrl = std::format(R"({}/{}.{}?{})",
			(std::string)s["sFlvUrl"],
			(std::string)s["sStreamName"],
			(std::string)s["sFlvUrlSuffix"],
			process_anticode(s["sFlvAntiCode"], GetanonymousUid(), s["sStreamName"]));
		return LiveStreamStatus::Normal;
	}
	else
	{
		return LiveStreamStatus::NotLive;
	}
	return LiveStreamStatus::Error;
}

std::string LiveHuya::GetLiveStreamLink()const
{
	return m_flvUrl;;
}

LiveHuya::~LiveHuya()
{

}

std::string LiveHuya::GetanonymousUid()
{
	std::string ret;
	const std::map <std::string, std::string>& header =
	{
		{"Content-Type","application/json"}
	};
	PostRequest(ret, live_huya_anonymous_uid, R"({"appId": 5002,"byPass": 3,"context": "", "version": "2.4","data": {}})", header);
	json::Json data;
	data.parse(ret);
	return data["data"]["uid"];
}

std::string LiveHuya::process_anticode(const std::string& anticode, const std::string& uid, const std::string& streamname)
{
	std::map<std::string, std::string> m = QueryStringToMap(urlDecode(anticode));
	m["ver"] = "1";
	m["sv"] = "2110211124";
	m["seqid"] = std::to_string(getCurrentUnixTime() * 1000 + std::stoll(uid));
	m["uid"] = uid;
	m["uuid"] = getUUID();
	const std::string& ss = Md5(std::format("{}|{}|{}", m["seqid"], m["ctype"], m["t"]));
	std::string fm = boost::beast::detail::base64_decode(m["fm"]);
	fm.replace(fm.find("$0"), 2, m["uid"]);
	fm.replace(fm.find("$1"), 2, streamname);
	fm.replace(fm.find("$2"), 2, ss);
	fm.replace(fm.find("$3"), 2, m["wsTime"]);
	m["wsSecret"] = Md5(fm);
	m.erase("fm");
	if (m.count("txyp") > 0)
		m.erase("txyp");
	return MapToQueryString(m);
}

std::string LiveHuya::getUUID()
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	auto value = now_ms.time_since_epoch().count();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1000);
	int rand_num = dis(gen);

	unsigned long long uuid = (value % 10000000000) * 1000 + rand_num;
	uuid %= 4294967295;

	return std::to_string(uuid);
}

LiveDoyin::LiveDoyin(const std::string& roomID) :m_roomID(roomID)
{

}

LiveStreamStatus::Status LiveDoyin::GetLiveStreamStatus()
{
	std::string ret;
	const std::map <std::string, std::string>& header =
	{
		{"User-Agent","Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/115.0.0.0 Safari/537.36 Edg/115.0.1901.203"},
		{"referer","https://live.douyin.com/"},
		{"cookie",""}
	};
	GetRequest(ret, std::format("{}/{}", live_douyin, m_roomID), header);
	// 抖音 status == 2 代表是开播的状态
	return LiveStreamStatus::Status();
}

std::string LiveDoyin::GetLiveStreamLink()
{
	return std::string();
}

LiveDoyin::~LiveDoyin()
{

}