#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

class CookieParser
{
public:
    CookieParser(const std::string_view CookieString)
    {
        size_t pos{};
        while (pos < CookieString.length())
        {
            size_t endPos = CookieString.find(';', pos);
            if (endPos == std::string::npos)
            {
                endPos = CookieString.length();
            }
            size_t equalPos = CookieString.find('=', pos);
            if (equalPos != std::string::npos && equalPos < endPos)
            {
                std::string key{ CookieString.substr(pos, equalPos - pos) };
                key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
                const std::string value{ CookieString.substr(equalPos + 1, endPos - equalPos - 1) };
                CookieMap[key] = value;
            }
            pos = endPos + 1;
        }
    }

    std::unordered_map<std::string, std::string>& GetCookieMap()
    {
        return CookieMap;
    }

    std::optional<std::string> GetCookieValue(const std::string_view key)
    {
        auto it{ CookieMap.find(key.data()) };
        if (it != CookieMap.end())
        {
            return std::make_optional(it->second);
        }
        else
        {
            return std::nullopt;
        }
    }

private:
    std::unordered_map<std::string, std::string> CookieMap;
};
