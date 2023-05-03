#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "HttpClient.h"
#include "json.h"
#include "Mihoyosdk.h"
class Login :HttpClient
{
public:
	Login(std::string output);
	~Login();
	void bh3Info();
	void putConfigFile();//临时先用着
	void setName();
	void scanQRCode(std::string& qrCode);
	void signedIn();
private:
	std::ifstream inFile;
	json::Json configJson;
	json::Json userInfo;
	std::stringstream configStringStream;
	int uid = 0;
	std::string access_key;
	std::string loginData;
	Mihoyosdk mihoyosdk;
	bool signed_in;
};