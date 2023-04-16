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
	userInfo.clear();
	configJson.clear();
	putconfigFile();
}
void Login::bh3Info()
{
	Info = m.verify(uid, access_key);

}
void Login::putconfigFile()//��ʱ������
{
	const std::string output = configJson.str();
	std::ofstream outFile("config_private_1.json");
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
	std::string realName = userInfo["uname"];
	configJson["realname"] = string_To_UTF8(realName);
	if (configJson["setname"] != "")
	{
		m.setUserName(configJson["realname"]);
	}
	else
	{
		m.setUserName(configJson["setname"]);
	}

}

void Login::scanQRCode(std::string& qrCode)
{
	m.scanCheck(qrCode, Info);
}

void Login::signedIn()
{
	bool signed_in = configJson["signed_in"];
	json::Json loginJ;
	Bsgsdk b;
	if (signed_in == true)
	{
		uid = configJson["uid"];
		access_key = configJson["access_key"];
	}
	else
	{
		loginJ.parse(b.login1(configJson["account"], configJson["password"]));
		uid = loginJ["uid"];
		access_key = loginJ["access_key"];
		loginJ.clear();
	}
	userInfo = b.getUserInfo(uid, access_key);
	m.getOAServer();
}