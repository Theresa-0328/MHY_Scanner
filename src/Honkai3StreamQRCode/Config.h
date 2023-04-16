#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "Core.h"
#include "Parser.h"
#include "Mihoyosdk.h"
class Config:Core
{
public:
	Config(std::string output);
	~Config();
	void bh3Info();
	void putconfigFile(const std::string& output);//临时先用着
	void setName();
	void scanQRCode(std::string& qrCode);
//private:
	std::ifstream inFile;
	json::Json configJson;
	json::Json userInfo;
	std::stringstream configStringStream;
	int uid = 0;
	std::string access_key;
	std::string Info;
	Mihoyosdk m;
	bool signed_in;
//private:
	void signedIn();
};