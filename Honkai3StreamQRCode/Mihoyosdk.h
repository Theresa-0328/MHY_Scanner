#pragma once
#include <string>
#include <iostream>

#include "Json.h"
#include "Parser.h"

class Mihoyosdk
{
public:
	std::string verify(std::string uid, std::string access_key);
private:
	const std::string verifyData = "{\"uid\":1, \"access_key\" :590}";
	const std::string verifyBody = "{\"device\":\"0000000000000000\", \"app_id\" : 1, \"channel_id\" : 14, \"data\" : {}, \"sign\" : \"\"}";
};

