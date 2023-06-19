#pragma once
#include "HttpClient.h"
class OfficialApi :public HttpClient
{
public:
	void request();
	void ConfirmRequest();
	int gameType;
	std::string ticket;
private:
	std::string generateUUID();
	std::map<std::string, std::string> headers = { {"cookie", "apple"}};
	std::string hkrpgFirst = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
	std::string hk4eFirst = "https://api-sdk.mihoyo.com//hk4e_cn/combo/panda/qrcode/scan";
	std::string getToken = "https://api-takumi.miyoushe.com/auth/api/getGameToken";

};