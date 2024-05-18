#pragma once

#include <string>
#include <format>
#include <random>
#include <sstream>

#include <Json.h>

#include "Common.h"
#include "HttpClient.h"

[[nodiscard]] inline std::string createUUID4()
{
    static const char chars[] = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);
    constexpr int UUID_LENGTH = 36;
    char uuid[UUID_LENGTH + 1]{};
    for (int i = 0; i < UUID_LENGTH; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            uuid[i] = '-';
            continue;
        }
        int randomDigit = distribution(generator);
        uuid[i] = chars[randomDigit];
    }
    uuid[UUID_LENGTH] = '\0';
    return std::string(uuid);
}

inline std::string GetLoginQrcodeUrl(const std::string& deviece, int gameTypeID = 4)
{
    HttpClient h;
    std::string res;
    h.PostRequest(res, hk4e_qrcode, std::format(R"({{"app_id":{},"device":"{}"}})", gameTypeID, deviece));
    json::Json j;
    j.parse(res);
    return static_cast<std::string>(j["data"]["url"]);
}