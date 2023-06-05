#pragma once
#include <QObject>
#include "Mihoyosdk.h"
#include "BiliGameApi.h"
#include <sstream>
#include <fstream>
class LoginBili
	: public QObject
	, public BiliGameApi
{
	Q_OBJECT
public:
	LoginBili(QObject* parent);
	~LoginBili();
	int loginBiliKey(std::string& realName);
	int loginBiliPwd(std::string Account, std::string Pwd, std::string& message);
	std::string getUName();
	void openConfig();
	void updateConfig();
	void setAutoStart(bool state);
	bool getAutoStart();
	void setAutoExit(bool Exit);
	bool getAutoExit();
	int uid = 0;
	std::string access_key;
	std::string capUrl;
private:
	std::ifstream inFile;
	json::Json configJson;
	std::string loginData;
	json::Json userInfo;
	bool autoExit = false;
};

