#include "OfficialApi.h"

#include <random>
#include <sstream>

#include <Json.h>

#include "CryptoKit.h"

std::string OfficialApi::getUid()const
{
	return cookieMap.at("login_uid");
}

void OfficialApi::setGameType(const GameType::Type gameType)
{
	m_gameType = gameType;
}

std::string OfficialApi::generateUUID()
{
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, 15);

	std::stringstream ss;
	for (int i = 0; i < 32; ++i)
	{
		int randomDigit = distribution(generator);
		ss << std::hex << randomDigit;
	}

	std::string uuid = ss.str();

	// 格式化UUID，插入分隔符
	uuid.insert(8, "-");
	uuid.insert(13, "-");
	uuid.insert(18, "-");
	uuid.insert(23, "-");

	return uuid;
}

std::string OfficialApi::getDS2()
{
	std::string time_now = std::to_string(getCurrentUnixTime());
	std::random_device rd;
	std::mt19937 gen(rd());
	// 定义范围
	int lower_bound = 100001;
	int upper_bound = 200000;
	std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
	std::string rand = std::to_string(dist(gen));
	std::string m = "salt=" + salt + "&t=" + time_now + "&r=" + rand;
	CryptoKit::Md5(m);
	return time_now + "," + rand + "," + m;
}

//扫码请求
int OfficialApi::scanRequest(const std::string& ticket, const std::string& uid, const std::string& token, const std::string& uuid)
{
	std::string scanUrl;
	std::string confirmUrl;
	json::Json payload;
	payload["app_id"] = m_gameType;
	payload["device"] = uuid;
	payload["ticket"] = ticket;
	switch (m_gameType)
	{
	case GameType::Honkai3:
		scanUrl = "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/scan";
		confirmUrl = "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/confirm";
		break;
	case GameType::Genshin:
		scanUrl = "https://api-sdk.mihoyo.com/hk4e_cn/combo/panda/qrcode/scan";
		confirmUrl = "https://api-sdk.mihoyo.com/hk4e_cn/combo/panda/qrcode/confirm";
		break;
	case GameType::StarRail:
		scanUrl = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
		confirmUrl = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/confirm";
		break;
	default:
		break;
	}
	std::string s;
	PostRequest(s, scanUrl, payload.str());
	json::Json j;
	j.parse(s);
	if ((int)j["retcode"] != 0)
	{
		return -1;
	}
	if (confirmRequest(uuid, ticket, uid, token, confirmUrl) != 0)
	{
		return -2;
	}
	return 0;
}

//扫码确认
int OfficialApi::confirmRequest(const std::string& UUID, const std::string& ticket,
	const std::string& uid, const std::string& token, const std::string& url)
{
	std::string s;
	json::Json payload;
	payload["proto"] = "Account";
	payload["raw"] = "{\\\"uid\\\":\\\"" + uid + "\\\",\\\"token\\\":\\\"" + token + "\\\"}";
	json::Json data;
	data["app_id"] = m_gameType;
	data["device"] = UUID;
	data["payload"] = payload;
	data["ticket"] = ticket;
	PostRequest(s, url, data.str());
	json::Json j;
	j.parse(s);
	if ((int)j["retcode"] != 0)
	{
		return -1;
	}
	return 0;
}

//获取用户完整信息
std::string OfficialApi::getUserName(std::string uid)
{
	std::string url = "https://bbs-api.miyoushe.com/user/api/getUserFullInfo";
	std::string re;
	url += "?uid=" + uid;
	GetRequest(re, url);
	json::Json j;
	j.parse(re);
	re = j["data"]["user_info"]["nickname"];
	return re;
}

std::string OfficialApi::getRole()
{
	std::string data;
	const std::string& url = "https://api-takumi.miyoushe.com/binding/api/getUserGameRolesByStoken";
	headers["DS"] = getDS2();
	headers["Cookie"] = "stuid=" + cookieMap.at("login_uid") + ";" + "stoken=" + data + ";" + "mid=" + "043co169fb_mhy";
	std::string re;
	GetRequest(re, url, headers);
	headers.erase("Cookie");
	return std::string();
}

int  OfficialApi::cookieParser(const std::string& cookieString)
{
	if (cookieString == "")
	{
		return -1;
	}
	// 切割 cookie 字符串
	size_t pos = 0;
	while (pos < cookieString.length())
	{
		// 查找键值对的结束位置
		size_t endPos = cookieString.find(';', pos);
		if (endPos == std::string::npos)
		{
			endPos = cookieString.length();
		}

		// 提取键值对
		size_t equalPos = cookieString.find('=', pos);
		if (equalPos != std::string::npos && equalPos < endPos)
		{
			std::string key = cookieString.substr(pos, equalPos - pos);
			key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
			std::string value = cookieString.substr(equalPos + 1, endPos - equalPos - 1);

			// 添加到字典中
			cookieMap[key] = value;
		}

		// 更新位置
		pos = endPos + 1;
	}
	if ((cookieMap.count("login_ticket") > 0) && (cookieMap.count("login_uid") > 0))
	{
		return 0;
	}
	return -1;
}

int OfficialApi::getMultiTokenByLoginTicket(std::string& data)
{
	std::string url = "https://api-takumi.mihoyo.com/auth/api/getMultiTokenByLoginTicket";
	std::map<std::string, std::string> params =
	{
		{"login_ticket",cookieMap.at("login_ticket")},
		{"uid",cookieMap.at("login_uid")},
		{"token_types","3"},
	};
	url = Url(url, params);
	std::string s;
	GetRequest(s, url);
	json::Json j;
	j.parse(s);
	if ((int)j["retcode"] == 0)
	{
		data = j["data"]["list"][0]["token"];
		return 0;
	}
	else
	{
		data = j["message"];
		return -1;
	}
}

int OfficialApi::getGameToken(const std::string& stoken, const std::string& uid, std::string& gameToken)
{
	std::string url = "https://api-takumi.mihoyo.com/auth/api/getGameToken";
	std::map<std::string, std::string> params =
	{
		{"stoken",stoken},
		{"uid",uid},
	};
	url = Url(url, params);
	std::string s;
	GetRequest(s, url);
	json::Json j;
	j.parse(s);
	gameToken = j["data"]["game_token"];
	return 0;
}