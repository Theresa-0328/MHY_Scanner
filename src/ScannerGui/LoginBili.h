#pragma once
#include <QObject>
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
	int uid = 0;
	std::string access_key;
	std::string capUrl;
private:
	json::Json configJson;
	std::string loginData;
	bool autoExit = false;
};

