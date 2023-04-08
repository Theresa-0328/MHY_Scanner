#pragma once
#include <string>
#include <iostream>

#include "Json.h"
#include "Parser.h"
#include "utils.h"

class Mihoyosdk
{
public:
	std::string verify(int uid, std::string access_key);
private:
	std::string makeSign(std::string);
	std::string bh3Sign(std::string);
private:
	utils u;
	const std::string loginV2Url = "https://api-sdk.mihoyo.com/bh3_cn/combo/granter/login/v2/login";
	std::map<std::string, std::string> verifyData = {{"uid","1"},{"access_key","590"}};
	const std::string verifyBody = "{\"device\":\"0000000000000000\",\"app_id\":1,\"channel_id\":14,\"data\":\"\",\"sign\":\"\"}";
};

