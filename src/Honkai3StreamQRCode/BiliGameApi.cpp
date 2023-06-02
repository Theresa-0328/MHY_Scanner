#include "BiliGameApi.h"
#include "CryptoKit.h"
std::string BiliGameApi::remove_quotes(std::string str)
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

std::string BiliGameApi::setSign(std::map<std::string, std::string> data)
{
    for (auto it = data.begin(); it != data.end(); ++it)
    {
        std::string key = it->first;
        std::string value = it->second;
        key.erase(std::remove(key.begin(), key.end(), '\"'), key.end());
        value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
        it->second = value;
    }

    data["timestamp"] = std::to_string(getCurrentUnixTime());
    data["client_timestamp"] = std::to_string(getCurrentUnixTime());
    std::string sign;
    std::string data2;
    for (std::pair<std::string, std::string> c : data)
    {
        if (c.first == "pwd")
        {
            std::string pwd = urlEncode(c.second);
            data2 += c.first+"="+pwd + "&";
        }
        data2 += c.first + "=" + c.second + "&";
    }
    for (std::pair<std::string, std::string> c : data)
    {
        sign += c.second;
    }
    sign = sign + "dbf8f1b4496f430b8a3c0f436a35b931";
    CryptoKit::Md5(sign);
    data2 += "sign=" + sign;
    return data2;
}

json::Json BiliGameApi::getUserInfo(const int uid,const std::string accessKey)
{
    std::string s;
    std::string t;
    json::Json j;
    j.parse(userinfoParam);
    j["uid"] = uid;
    j["access_key"] = accessKey;
    std::map < std::string, std::string > m = j.objToMap();
    for (auto& a : m)
    {
        a.second = remove_quotes(a.second);
    }
    s = setSign(m);
    PostRequest(t,bililogin + "api/client/user.info", s, headers);
    t = UTF8_To_string(t);
#ifdef _DEBUG
    std::cout << "BiliBili用户信息：" << t << std::endl;
#endif // _DEBUG
    j.clear();
    j.parse(t);
    return j;
}

std::string BiliGameApi::biliLogin(const std::string& biliAccoun, const std::string& biliPwd, bool cap)
{
    if (!cap)
    {
        captcha();
        make_captch();
        return "";//loginWithCaptcha
    }
    else
    {
        return loginWithoutCaptcha(biliAccoun, biliPwd);
    }
    
}

std::string BiliGameApi::captcha()
{
    json::Json data;
    data.parse(captchaParam);
    std::map < std::string, std::string > info = data.objToMap();
    std::string data1 = setSign(info);
    std::string data2;
    HttpClient::PostRequest(data2, bililogin + "api/client/start_captcha", data1, headers);
    captchaJ.parse(data2);
    return std::string();
}

std::string BiliGameApi::make_captch()
{
    std::string gt = captchaJ["gt"];
    std::string challenge = captchaJ["challenge"];
    std::string gt_user = captchaJ["gt_user_id"];
    std::string capurl = "http://127.0.0.1:12983/?captcha_type=1&challenge=" + challenge + "&gt=" + gt + "&userid=" + gt_user + "&gs=1";
    //capurl = "https://game.bilibili.com/sdk/geetest/?captcha_type=1&challenge=" + challenge + "&gt=" + gt + "&userid=" + gt_user + "&gs=1";
    return std::string();
}

std::string BiliGameApi::loginWithoutCaptcha(const std::string& biliAccount, const std::string& biliPwd)
{
    json::Json data;
    data.parse(rsaParam);
    std::map < std::string, std::string> dataM= data.objToMap();
    std::string p1 = setSign(dataM);
    std::string re;
    PostRequest(re, bililogin + "api/client/rsa", p1,headers);
#ifdef _DEBUG
    std::cout << re << std::endl;
#endif // _DEBUG
    data.clear();
    data.parse(loginParam);
    json::Json re1J;
    re1J.parse(re);
    std::string publicKey = re1J["rsa_key"];
    CryptoKit::FormatRsaPublicKey(publicKey);
    data["access_key"] = "";
    data["gt_user_id"] = "";
    data["uid"] = "";
    data["challenge"] = "";
    data["user_id"] = biliAccount;
    data["validate"] = "";
    std::string hash1 =  re1J["hash"];
    std::string rekit = CryptoKit::rsaEncrypt(hash1 + biliPwd, publicKey);
    data["pwd"] = rekit;
#ifdef _DEBUG
    std::cout << data.str() << std::endl;
#endif // _DEBUG
    std::map < std::string, std::string> dataR = data.objToMap();
    std::string p2 = setSign(dataR);
    re.clear(); 
    PostRequest(re, bililogin + "api/client/login", p2, headers);
    data.clear();
    re1J.clear();
    return re;
}