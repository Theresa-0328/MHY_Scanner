#include "LoginBili.h"
#include <QMessageBox>
#include "ThreadLocalServer.h"
#include <qdesktopservices.h>
#include <QUrl>
LoginBili::LoginBili(QObject* parent)
    : QObject(parent)
{

}

LoginBili::~LoginBili()
{
    inFile.close();
}

void LoginBili::openConfig()
{
    inFile.open("./Config/config_private.json");
    std::stringstream configStringStream;
    configStringStream << inFile.rdbuf();
    const std::string& configString = configStringStream.str();
    configJson.parse(configString);
}

void LoginBili::updateConfig()//先放这里
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

void LoginBili::setAutoStart(bool state)
{
    if (state)
    {
        configJson["is_auto_start"] = true;
    }
    else
    {
        configJson["is_auto_start"] = false;
    }
    updateConfig();
}

bool LoginBili::getAutoStart()
{
    return (bool)configJson["is_auto_start"];
}

void LoginBili::setAutoExit(bool Exit)
{
    
    if (Exit)
    {
        configJson["is_auto_exit"] = true;
    }
    else
    {
        configJson["is_auto_exit"] = false;
    }
    updateConfig();
}

bool LoginBili::getAutoExit()
{
    return (bool)configJson["is_auto_exit"];;
}

//获取用户名,检查access_key是否有效并更新配置文件
int LoginBili::loginBiliKey(std::string& realName)
{
    if (!configJson["signed_in"])
    {
        return -1;
    }
    uid = configJson["uid"];
    access_key = configJson["access_key"];
    json::Json userInfo = getUserInfo(uid, access_key);
    int code = (int)userInfo["code"];
    if (code != 0)
    {
        configJson["signed_in"] = false;
        updateConfig();
        return code;
    }
    uid = (int)userInfo["uid"];
    configJson["uid"] = uid;
    realName = HttpClient::string_To_UTF8(userInfo["uname"]);
    configJson["realname"] = realName;
    configJson["signed_in"] = true;
    updateConfig();
    return 0;
}

int LoginBili::loginBiliPwd(std::string Account, std::string Pwd, std::string& message)
{
    json::Json loginJ;
    std::string loginInfo = login(Account, Pwd);
    loginInfo = HttpClient::UTF8_To_string(loginInfo);
    loginJ.parse(loginInfo);
    int code = (int)loginJ["code"];
    if (code == 200000)
    {
        ThreadLocalServer thserver;
        thserver.start();
        const std::string capUrl = makeCaptchUrl();
        QString URL = QString::fromStdString(capUrl);
        QDesktopServices::openUrl(QUrl(URL.toLatin1()));
        QMessageBox::information(nullptr, "提示", "完成验证码后再点击OK。");
        thserver.stop();
        json::Json captcha;
        captcha.parse(thserver.reCaptcha());
        std::string challenge = captcha["challenge"];
        std::string gt_user = captcha["userid"];
        std::string validate = captcha["validate"];
        loginInfo = login(Account, Pwd,challenge, gt_user, validate);
        loginInfo = HttpClient::UTF8_To_string(loginInfo);
        loginJ.parse(loginInfo);
        code = (int)loginJ["code"];
    }
    if (code != 0)
    {
        message = loginJ["message"];
        updateConfig();
        return code;
    }
    configJson["account"] = Account;
    configJson["password"] = Pwd;
    uid = loginJ["uid"];
    access_key = loginJ["access_key"];
    configJson["uid"] = uid;
    configJson["access_key"] = access_key;
    loginJ.clear();
    configJson["signed_in"] = true;
    userInfo = getUserInfo(uid, access_key);
    configJson["realname"] = HttpClient::string_To_UTF8(userInfo["uname"]);
    updateConfig();
    return code;
}

std::string LoginBili::getUName()
{
    return configJson["realname"];
}