#include "OfficialApi.h"

#include <random>
#include <sstream>
#include <format>

#include <Json.h>

#include "CryptoKit.h"

OfficialApi::OfficialApi()
{
	scanUrl.reserve(100);
	confirmUrl.reserve(100);
}



std::string OfficialApi::getUid()const
{
	return cookieMap.at("login_uid");
}

bool OfficialApi::scanInit(const GameType::Type gameType, const std::string& ticket, const std::string& uid, const std::string& gameToken)
{
	std::unique_lock<std::shared_mutex> lock(mutex);
	if (ticket == m_ticket)
	{
		return false;
	}
	switch (gameType)
	{
	case GameType::Honkai3:
		scanUrl = mhy_bh3_qrcode_scan;
		confirmUrl = mhy_bh3_qrcode_confirm;
		break;
	case GameType::Genshin:
		scanUrl = mhy_hk4e_qrcode_scan;
		confirmUrl = mhy_hk4e_qrcode_confirm;
		break;
	case GameType::StarRail:
		scanUrl = mhy_hkrpg_qrcode_scan;
		confirmUrl = mhy_hkrpg_qrcode_confirm;
		break;
	default:
		break;
	}
	uuid = generateUUID();
	m_uid = uid;
	m_ticket = ticket;
	m_gameToken = gameToken;
	return true;
}

//暂时用不上
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
	Md5(m);
	return time_now + "," + rand + "," + m;
}

//扫码请求
ScanRet::Type OfficialApi::scanRequest()
{
	std::string m_sacnRet{};
	PostRequest(m_sacnRet, scanUrl, std::format(R"({{"app_id":{},"device":"{}","ticket":"{}"}})", static_cast<int>(m_gameType), uuid, m_ticket));
	json::Json j;
	j.parse(m_sacnRet);
	if ((int)j["retcode"] != 0)
	{
		return ScanRet::Type::FAILURE_1;
	}
	return ScanRet::Type::SUCCESS;
}

//扫码确认
ScanRet::Type OfficialApi::scanConfirm()
{
	std::string s;
	json::Json payload;
	payload["proto"] = "Account";
	payload["raw"] = std::format(R"({{\"uid\":\"{}\",\"token\":\"{}\"}})", m_uid, m_gameToken);
	json::Json data;
	data["app_id"] = m_gameType;
	data["device"] = uuid;
	data["payload"] = payload;
	data["ticket"] = m_ticket;
	PostRequest(s, confirmUrl, data.str());
	json::Json j;
	j.parse(s);
	if ((int)j["retcode"] != 0)
	{
		return ScanRet::Type::FAILURE_2;
	}
	return ScanRet::Type::SUCCESS;
}

//获取用户完整信息
std::string OfficialApi::getUserName(const std::string& uid)
{
	std::string re;
	GetRequest(re, std::format("{}?uid={}", mhy_mys_uesrinfo, uid));
	json::Json j;
	j.parse(re);
	re = j["data"]["user_info"]["nickname"];
	return re;
}

//暂时用不上
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
			const std::string& value = cookieString.substr(equalPos + 1, endPos - equalPos - 1);
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

std::string OfficialApi::generateUUID()
{
	static const char chars[] = "0123456789abcdef";
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<int> distribution(0, 16);
	constexpr int UUID_LENGTH = 36;
	char uuid[UUID_LENGTH + 1]{};
	for (int i = 0; i < UUID_LENGTH; ++i)
	{
		if (i == 8 || i == 13 || i == 18 || i == 23)
		{
			uuid[i] = '-';
		}
		else
		{
			int randomDigit = distribution(generator);
			uuid[i] = chars[randomDigit];
		}
	}
	uuid[UUID_LENGTH] = '\0';
	return std::string(uuid);
}

int OfficialApi::getMultiTokenByLoginTicket(std::string& data)
{
	std::map<std::string, std::string> params =
	{
		{"login_ticket",cookieMap.at("login_ticket")},
		{"uid",cookieMap.at("login_uid")},
		{"token_types","3"},
	};
	std::string s;
	GetRequest(s, std::format("{}?{}", mhy_takumi_multi_token_by_login_ticket, MapToQueryString(params)));
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
	std::map<std::string, std::string> params =
	{
		{"stoken",stoken},
		{"uid",uid},
	};
	std::string s;
	GetRequest(s, std::format("{}?{}", mhy_takumi_game_token, MapToQueryString(params)));
	const std::string& data = UTF8_To_string(s);
	json::Json j;
	j.parse(data);
	int retcode = j["retcode"];
	if (retcode == 0)
	{
		gameToken = j["data"]["game_token"];
	}
	return retcode;
}