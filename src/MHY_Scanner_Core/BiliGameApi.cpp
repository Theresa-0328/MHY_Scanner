#include "BiliGameApi.h"

#include <format>

#include "CryptoKit.h"
#include "UtilString.hpp"

std::string BiliGameApi::remove_quotes(std::string str)
{
	std::string result;
	char* p = &str[0];
	while (*p != '\0')
	{
		if (*p != '"')
		{
			result += *p;
		}
		p++;
	}
	return result;
}

std::string BiliGameApi::setSign(std::map<std::string, std::string> data)
{
	for (auto it = data.begin(); it != data.end(); ++it)
	{
		std::string key = it->first;
		std::string value = it->second;
		key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
		value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
		it->second = value;
	}

	data["timestamp"] = std::to_string(getCurrentUnixTime());
	data["client_timestamp"] = std::to_string(getCurrentUnixTime());
	std::string sign;
	std::string data2;
	for (std::pair<std::string, std::string> c : data)
	{
		if (c.first == "pwd")
		{
			std::string pwd = urlEncode(c.second);
			data2 += c.first + "=" + pwd + "&";
		}
		data2 += c.first + "=" + c.second + "&";
	}
	for (std::pair<std::string, std::string> c : data)
	{
		sign += c.second;
	}
	sign += "dbf8f1b4496f430b8a3c0f436a35b931";
	return std::format("{}{}{}", data2, "sign=", Md5(sign));
}

std::string BiliGameApi::getUserInfo(const std::string& uid, const std::string& accessKey)
{
	json::Json j;
	j.parse(userinfoParam);
	j["uid"] = uid;
	j["access_key"] = accessKey;
	std::map < std::string, std::string > m = j.objToMap();
	for (auto& it : m)
	{
		it.second = remove_quotes(it.second);
	}
	std::string s = setSign(m);
	std::string t;
	PostRequest(t, game_bili_userinfo, s, headers);
#ifdef _DEBUG
	std::cout << "BiliBili用户信息：" << t << std::endl;
#endif // _DEBUG
	return t;
}

void BiliGameApi::captcha()
{
	json::Json data;
	data.parse(captchaParam);
	std::map < std::string, std::string > info = data.objToMap();
	std::string data1 = setSign(info);
	std::string data2;
	HttpClient::PostRequest(data2, game_bili_start_captcha, data1, headers);
	captchaJ.parse(data2);
}

std::string BiliGameApi::makeCaptchUrl()
{
	captcha();
	const std::string& gt = captchaJ["gt"];
	const std::string& challenge = captchaJ["challenge"];
	const std::string& gt_user_id = captchaJ["gt_user_id"];
	//NOTE:official https://game.bilibili.com/sdk/geetest/?captcha_type=1&challenge=" + challenge + "&gt=" + gt + "&userid=" + gt_user + "&gs=1
	std::string capurl = std::format(R"=(http://127.0.0.1:12983/?captcha_type=1&challenge={}&gt={}&userid={}&gs=1)=", challenge, gt, gt_user_id);
	return capurl;
}

std::string BiliGameApi::login(const std::string& biliAccount, const std::string& biliPwd)
{
	json::Json data;
	data.parse(rsaParam);
	std::map < std::string, std::string> dataM = data.objToMap();
	std::string p1 = setSign(dataM);
	std::string re;
	PostRequest(re, game_bili_rsa, p1, headers);
#ifdef _DEBUG
	std::cout << re << std::endl;
#endif // _DEBUG
	data.parse(loginParam);
	json::Json re1J;
	re1J.parse(re);
	std::string publicKey = re1J["rsa_key"];
	FormatRsaPublicKey(publicKey);
	data["access_key"] = "";
	data["gt_user_id"] = "";
	data["uid"] = "";
	data["challenge"] = "";
	data["user_id"] = biliAccount;
	data["validate"] = "";
	std::string hash1 = re1J["hash"];
	std::string rekit = rsaEncrypt(hash1 + biliPwd, publicKey);
	data["pwd"] = rekit;
#ifdef _DEBUG
	std::cout << data.str() << std::endl;
#endif // _DEBUG
	std::map < std::string, std::string> dataR = data.objToMap();
	std::string p2 = setSign(dataR);
	re.clear();
	PostRequest(re, game_bili_login, p2, headers);
	data.clear();
	re1J.clear();
	return re;
}

std::string BiliGameApi::login(const std::string& biliAccount, const std::string& biliPwd,
	const std::string challenge, const std::string gt_user, const std::string validate)
{
	json::Json data;
	data.parse(rsaParam);
	std::map < std::string, std::string> dataM = data.objToMap();
	std::string p1 = setSign(dataM);
	std::string re;
	PostRequest(re, game_bili_rsa, p1, headers);
#ifdef _DEBUG
	std::cout << re << std::endl;
#endif // _DEBUG
	data.parse(loginParam);
	json::Json re1J;
	re1J.parse(re);
	std::string publicKey = re1J["rsa_key"];
	FormatRsaPublicKey(publicKey);
	data["access_key"] = "";
	data["gt_user_id"] = gt_user;
	data["uid"] = "";
	data["challenge"] = challenge;
	data["user_id"] = biliAccount;
	data["validate"] = validate;
	data["seccode"] = validate + "|jordan";
	std::string hash1 = re1J["hash"];
	std::string rekit = rsaEncrypt(hash1 + biliPwd, publicKey);
	data["pwd"] = rekit;
#ifdef _DEBUG
	std::cout << data.str() << std::endl;
#endif // _DEBUG
	std::map < std::string, std::string> dataR = data.objToMap();
	std::string p2 = setSign(dataR);
	re.clear();
	PostRequest(re, game_bili_login, p2, headers);
	data.clear();
	re1J.clear();
	return re;
}