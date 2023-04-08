#include "Mihoyosdk.h"
#include "HmacSha256.hpp"
std::string Mihoyosdk::verify(int uid, std::string access_key)
{
	std::cout << "verify with uid="<<uid << std::endl;
	verifyData["uid"] = std::to_string(uid);
	verifyData["access_key"] = access_key;
	std::string ssd = "{\\\"access_key\\\":\\\"" + verifyData["access_key"]+ "\\\""",\\\"uid\\\":"+ verifyData["uid"]+"}";
	json::Json body;
	body.parse(verifyBody);
	body["data"] = ssd;
	std::cout << body.str() << std::endl;
	makeSign(body.str());
	std::string s;
	u.PostRequest(s, loginV2Url, makeSign(body.str()));
	return s;
}

std::string Mihoyosdk::makeSign(std::string data)
{
	std::string sign;
	std::string	data2;
	json::Json p;
	p.parse(data);
	std::map<std::string,std::string> m= p.objToMap();
	for (auto &it: m)
	{
		if (it.first == "sign")
			continue;
		if(it.first=="data")
		{
			if (it.second.front() == '\"')
				it.second.erase(0, 1);
			if (it.second.back() == '\"')
				it.second.pop_back();
		}
		if(it.first =="device")
		{
			if (it.second.front() == '\"')
				it.second.erase(0, 1);
			if (it.second.back() == '\"')
				it.second.pop_back();
		}
		data2 += it.first + "=" + it.second + "&";
	}
	data2.erase(data2.length() - 1);
	std::cout <<"data2=" << data2 << std::endl;
	sign = bh3Sign(data2);
	p["sign"] = sign;
	std::cout << " p.str()=" << p.str() << std::endl;
	return p.str();
}

std::string Mihoyosdk::bh3Sign(std::string data)
{
	std::cout << data << std::endl;
	std::string key = "0ebc517adb1b62c6b408df153331f9aa";
	data.erase(std::remove(data.begin(), data.end(), '\\'), data.end());
	std::cout << data << std::endl;
	std::string sign = hmac_sha256(key, data);
	return sign;
}
