#pragma once
#include "Mihoyosdk.h"
#include "Bsgsdk.h"
#include <sstream>
#include <fstream>
class LoginBili :public Mihoyosdk, public Bsgsdk
{
public:
	int loginBiliBiliKey();
	int loginBiliBiliPwd(std::string Account, std::string Pwd, std::string& message);
	void loginHonkai3();
	void openConfig();
	void updateConfig();
	void scanQRCode(std::string qrCode);
private:
	std::ifstream inFile;
	json::Json configJson;
	int uid;
	std::string access_key;
	std::string loginData;
};

