#include "Bsgsdk.h"

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

std::string Bsgsdk::setSign(std::map<std::string, std::string> data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        std::string key = it->first;
        std::string value = it->second;
        key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
        it->second = value;
    }

    data["timestamp"] = std::to_string(u.getCurrentUnixTime());
    data["client_timestamp"] = std::to_string(u.getCurrentUnixTime());
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
    u.PostRequest(t,bililogin + "api/client/user.info", s, headers);
    t = u.UTF8_To_string(t);
    std::cout <<"BiliBili验证成功：" << t << std::endl;
    return j;
}

std::string Bsgsdk::login1(const std::string& biliAccoun, const std::string& biliPwd, bool cap)
{
    if (cap)
    {

    }
    else
    {
        return login2(biliAccoun, biliPwd);
    }
    
}

std::string Bsgsdk::login2(const std::string& biliAccount, const std::string& biliPwd)
{
    json::Json data;
    data.parse(rsaParam);
    std::map < std::string, std::string> dataM= data.objToMap();
    std::string p1 = setSign(dataM);
    std::string re;
    u.PostRequest(re, bililogin + "api/client/rsa", p1,headers);
    std::cout << re<< std::endl;
    data.clear();
    data.parse(loginParam);
    json::Json re1J;
    re1J.parse(re);
    std::string publicKey = re1J["rsa_key"];
    publicKey = kit.formatRsaPublicKey(publicKey);
    data["access_key"] = "";
    data["gt_user_id"] = "";
    data["uid"] = "";
    data["challenge"] = "";
    data["user_id"] = biliAccount;
    data["validate"] = "";
    std::string hash1 =  re1J["hash"];
    std::string rekit = kit.rsaEncrypt(hash1 + biliPwd, publicKey);
    data["pwd"] = rekit;
    std::cout << data.str() << std::endl;
    std::map < std::string, std::string> dataR = data.objToMap();
    std::string p2 = setSign(dataR);
    re.clear(); 
    u.PostRequest(re, bililogin + "api/client/login", p2, headers);
    return re;
}