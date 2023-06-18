#pragma once
#include "HttpClient.h"
class OfficialApi :public HttpClient
{
public:
	void request(std::string ticket);
private:
	std::string cookie;
	std::string hkrpgFirst = "api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
	std::string hk4eFirst = "api-sdk.mihoyo.com//hk4e_cn/combo/panda/qrcode/scan";
};