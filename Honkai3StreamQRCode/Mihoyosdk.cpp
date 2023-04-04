#include "Mihoyosdk.h"

std::string Mihoyosdk::verify(std::string uid, std::string access_key)
{
	std::cout << "verify with uid="<<uid << std::endl;
	json::Json data;
	data.parse(verifyData);
	data["uid"] = uid;
	data["access_key"] = access_key;
	json::Json body;
	body.parse(verifyBody);
	body["data"] = data;
	//std::cout << body.str() << std::endl;
	makeSign(body.str());
	return std::string();
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
		data2 += it.first + "=" + it.second + "&";
	}
	data2.erase(data2.length() - 1);
	std::cout << data2 << std::endl;
	sign = bh3Sign(data2);
	return std::string();
}

std::string Mihoyosdk::bh3Sign(std::string data)
{
	std::string key = "0ebc517adb1b62c6b408df153331f9aa";
	std::string sign;
	//hmac_sha256
	return sign;
}
