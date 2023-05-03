#include "LoginBili.h"
void LoginBili::openConfig()
{
    inFile.open("./Config/config_private.json");
    std::stringstream configStringStream;
    configStringStream << inFile.rdbuf();
    const std::string& configString = configStringStream.str();
    configJson.parse(configString);
}

void LoginBili::updateConfig()
{
    const std::string output = configJson.str();
    std::ofstream outFile("./Config/config_private.json");
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

void LoginBili::scanQRCode(std::string qrCode)
{
    scanCheck(qrCode, loginData);
}

int LoginBili::loginBiliBiliKey()
{
    uid = configJson["uid"];
    access_key = configJson["access_key"];
    json::Json userInfo = getUserInfo(uid, access_key);
    std::string realName = HttpClient::string_To_UTF8(userInfo["uname"]);
    setUserName(realName);
    return 0;
}

int LoginBili::loginBiliBiliPwd(std::string Account, std::string Pwd, std::string& message)
{
    std::string a = login1(Account, Pwd);
    json::Json j;
    j.parse(a);
    //access_key" uid"
    message = HttpClient::UTF8_To_string(j["message"]);//code!=0
    return (int)j["code"];
}

void LoginBili::loginHonkai3()
{
    loginData = verify(uid, access_key);
}