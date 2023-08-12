#pragma once

#include "BiliGameApi.h"

#include <sstream>
#include <fstream>

class LoginBili
	:public BiliGameApi
{
public:
	LoginBili();
	~LoginBili();
	int loginBiliKey(std::string& name, const std::string& uid, const std::string& access_key);
	int loginBiliPwd(std::string Account, std::string Pwd,
		std::string& message, std::string& uid, std::string& access_key, std::string& name);
};