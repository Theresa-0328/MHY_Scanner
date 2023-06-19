#include "OfficialApi.h"
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

//扫码请求
void OfficialApi::request()
{
	json::Json payload;
	payload["app_id"] = 8;
    payload["device"] = generateUUID();
    payload["ticket"] = ticket;
    std::string s;
    PostRequest(s, hkrpgFirst,payload.str());
}

//扫码确认
void OfficialApi::ConfirmRequest()
{
    std::string s;
    GetRequest(s, hkrpgFirst, headers);
    json::Json j;
    j.parse(s);
}