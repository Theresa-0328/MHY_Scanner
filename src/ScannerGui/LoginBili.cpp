#include "LoginBili.h"
#include <QMessageBox>
#include "ThreadLocalServer.h"
#include <qdesktopservices.h>
#include <QUrl>
#include "Mihoyosdk.h"
LoginBili::LoginBili(QObject* parent)
    : QObject(parent)
{

}

LoginBili::~LoginBili()
{

}

void LoginBili::openConfig()
{
    std::ifstream inFile;
    inFile.open("./Config/config_private.json");
    std::stringstream configStringStream;
    configStringStream << inFile.rdbuf();
    const std::string& configString = configStringStream.str();
    configJson.parse(configString);
    Mihoyosdk m;
    m.setBHVer("6.7.0");
    m.setOAServer();
    inFile.close();
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
        QMessageBox msgBox;
        msgBox.setWindowTitle("验证提示");
        msgBox.setTextInteractionFlags(Qt::TextSelectableByMouse);
        msgBox.setText("如果没有自动打开浏览器，请手动复制并打开后面的链接并进行验证"+URL+ "如果你完成了验证码，点击OK");
        msgBox.exec();
        thserver.stop();
        if (thserver.reCaptcha() == "")
        {
            message = loginJ["message"];
            updateConfig();
            return code;
        }
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