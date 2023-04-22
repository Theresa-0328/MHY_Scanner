#include "Bsgsdk.h"
#include "Login.h"

Login::Login(std::string output)
{
	inFile.open(output);
	if (inFile)
	{
		configStringStream << inFile.rdbuf();
		const std::string& configString = configStringStream.str();
		configJson.parse(configString);
	}
	else
	{
		inFile.open("config.json");
		configStringStream << inFile.rdbuf();
		const std::string& configString = configStringStream.str();
		configJson.parse(configString);
	}
	signed_in = configJson["signed_in"];
}
Login::~Login()
{
	putConfigFile();
	userInfo.clear();
	configJson.clear();
}
void Login::bh3Info()
{
	loginData = mihoyosdk.verify(uid, access_key);

}
void Login::putConfigFile()//ÁÙÊ±ÏÈÓÃ×Å
{
	const std::string output = configJson.str();
	std::ofstream outFile("config_private.json");
	std::stringstream outStr;
	bool isInPair = false;
	for (int i = 0; i < output.size(); i++)
	{
		if (output[i] == '{')
		{
			outStr << "{\n";
			continue;
		}
		if (output[i] == '}')
		{
			outStr << "\n}";
			isInPair = false;
			continue;
		}
		if (output[i] == ',')
		{
			outStr << ",\n";
			isInPair = false;
			continue;
		}
		if (!isInPair)
		{
			outStr << "  ";
			isInPair = true;
		}
		outStr << output[i];
	}
	outFile << outStr.str();
	outFile.close();
}

void Login::setName()
{
	std::string realName = string_To_UTF8(userInfo["uname"]);
	configJson["realname"] = realName;
	if (configJson["setname"] != "")
	{
		mihoyosdk.setUserName(realName);
	}
	else
	{
		mihoyosdk.setUserName(configJson["setname"]);
	}
}

void Login::scanQRCode(std::string& qrCode)
{
	mihoyosdk.scanCheck(qrCode, loginData);
}

void Login::signedIn()
{
	bool signed_in = configJson["signed_in"];
	json::Json loginJ;
	Bsgsdk b;
	if (signed_in == true)
	{
		std::cout << "bilibili»º´æÕËºÅµÇÂ¼ÖÐ"<<std::endl;
		uid = configJson["uid"];
		access_key = configJson["access_key"];
		userInfo = b.getUserInfo(uid, access_key);
	}
	if(signed_in == false||(int)userInfo["code"] != 0)//access_keyÊ§Ð§£¬³¢ÊÔÖØÐÂµÇÂ¼
	{
		std::cout << "bilibili»º´æÕËºÅÎÞÐ§£¬³¢ÊÔÃÜÂëµÇÂ¼ÖÐ" << std::endl;
		loginJ.parse(b.login1(configJson["account"], configJson["password"]));
		if ((int)loginJ["code"] != 0)
		{
			std::cout << "ÕËºÅ»òÃÜÂë´íÎó£¬µÇÂ¼Ê§°Ü£¬ÈÎÒâ¼üÍË³ö" << std::endl;
			system("pause");
			exit(-1);
		}
		uid = loginJ["uid"];
		access_key = loginJ["access_key"];
		configJson["uid"] = uid;
		configJson["access_key"] = access_key;
		loginJ.clear();
		configJson["signed_in"] = true;
		userInfo = b.getUserInfo(uid, access_key);
	}
	std::cout << "bilibiliµÇÂ¼Íê³É£¡" << std::endl;
}
