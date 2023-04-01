#include "Bsgsdk.h"
//#include "HmacSha256_.cpp"
std::string Bsgsdk::remove_quotes(std::string str)
{
    std::string result = "";
    char* p = &str[0];
    while (*p != '\0') 
    {
        if (*p != '"') 
        {
            result += *p;
        }
        p++;
    }
    return result;
}

std::string Bsgsdk::setSign(std::map<std::string, std::string>data)
{
    int t = u.getCurrentUnixTime();
    data["timestamp"] = std::to_string(t);
    data["client_timestamp"] = std::to_string(t);
    std::string sign;
    std::string data2;
    for (std::pair<std::string, std::string> c : data)
    {
        if (c.first == "pwd")
        {
            std::string pwd = u.urlEncode(c.second);
            data2 += c.first+"="+pwd + "&";
        }
        data2 += c.first + "=" + c.second + "&";
    }
    for (std::pair<std::string, std::string> c : data)
    {
        sign += c.second;
    }
    sign = sign + "dbf8f1b4496f430b8a3c0f436a35b931";
    sign = md5(sign);
    data2 += "sign=" + sign;
    return data2;
}

json::Json Bsgsdk::getUserInfo(std::string uid,std::string accessKey)
{
    std::string s;
    std::string t;
    json::Json j;
    j.parse(userinfoParam);
    j["uid"] = uid;
    j["access_key"] = accessKey;
    std::map < std::string, std::string > m;
    m = j.objToMap();
    for (auto& a : m)
    {
        a.second = remove_quotes(a.second);
    }
    s = setSign(m);
    std::map<std::string, std::string> headers = 
    { 
        {"User-Agent","Mozilla/5.0 BSGameSDK"}, 
        {"Content-Type","application/x-www-form-urlencoded"}, 
        {"Host","line1-sdk-center-login-sh.biligame.net"} 
    };
    u.PostRequest(t,bililogin + "api/client/user.info", s, headers);
    t = u.UTF8_To_string(t);
    std::cout << t << std::endl;
    return j;
}

//json::Json Bsgsdk::login()
//{
//    return json::Json();
//}