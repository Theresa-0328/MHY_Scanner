#include "OfficialApi.h"
#include "CryptoKit.h"
#include "Json.h"
#include <random>
#include <sstream>
#include <iomanip>

std::string OfficialApi::generateUUID() 
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);

    std::stringstream ss;
    for (int i = 0; i < 32; ++i) 
    {
        int randomDigit = distribution(generator);
        ss << std::hex << randomDigit;
    }

    std::string uuid = ss.str();

    // 格式化UUID，插入分隔符
    uuid.insert(8, "-");
    uuid.insert(13, "-");
    uuid.insert(18, "-");
    uuid.insert(23, "-");

    return uuid;
}

std::string OfficialApi::getDS()
{
    std::string time_now = std::to_string(getCurrentUnixTime());
    std::random_device rd;
    std::mt19937 gen(rd());
    // 定义范围
    int lower_bound = 100001;
    int upper_bound = 200000;
    std::uniform_int_distribution<int> dist(lower_bound, upper_bound);
    std::string rand = std::to_string(dist(gen));
    std::string m = "salt=" +salt +"&t=" + time_now +"&r=" + rand;
    CryptoKit::Md5(m);
    return time_now+","+rand +"," + m;
}

//扫码请求
void OfficialApi::scanRequest(std::string UUID)
{
    json::Json payload;
    payload["app_id"] = 8;
    payload["device"] = UUID;
    payload["ticket"] = ticket;
    std::string s;
    std::string hkrpgFirst = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/scan";
    PostRequest(s, hkrpgFirst,payload.str());
}

//扫码确认
void OfficialApi::confirmRequest(std::string UUID)
{
    std::string uid = "";
    std::string token = "";
    std::string s;
    std::string getToken = "https://api-sdk.mihoyo.com/hkrpg_cn/combo/panda/qrcode/confirm";
    json::Json payload;
    payload["proto"] = "Account";
    payload["raw"] = "{\\\"uid\\\":\\\""+uid+"\\\",\\\"token\\\":\\\""+ token+"\\\"}";
    json::Json data;
    data["app_id"] = 8;
    data["device"] = UUID;
    data["payload"] = payload;
    data["ticket"] = ticket;
    std::string d = data.str();
    PostRequest(s, getToken, d);
    s = UTF8_To_string(s);
    json::Json j;
    j.parse(s);
}

void OfficialApi::scanLogin()
{
    std::string UUID = generateUUID();
    scanRequest(UUID);
    confirmRequest(UUID);
}

//获取绑定的账号
void OfficialApi::getRole()
{
    std::string url = "https://api-takumi.miyoushe.com/binding/api/getUserGameRolesByStoken";
    headers["DS"] = getDS();
    headers["Cookie"] = "";
    std::string re;
    GetRequest(re, url, headers);
    re = UTF8_To_string(re);
    string_To_UTF8(re);
}