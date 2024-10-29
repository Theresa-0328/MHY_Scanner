#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>

class CookieParser
{
public:
    explicit CookieParser(const std::string_view CookieString)
    {
        size_t pos{};
        while (pos < CookieString.length())
        {
            size_t equalPos{ CookieString.find('=', pos) };
            if (equalPos == std::string::npos)
            {
                break;
            }
            size_t endPos{ CookieString.find(';', pos) };
            if (endPos == std::string::npos)
            {
                endPos = CookieString.length();
            }
            std::string_view key{ CookieString.substr(pos, equalPos - pos) };
            std::string_view value{ CookieString.substr(equalPos + 1, endPos - equalPos - 1) };
            key = key.substr(key.find_first_not_of(' '), key.find_last_not_of(' ') + 1);
            CookieMap.emplace(key, value);
            pos = endPos + 1;
        }
    }

    std::unordered_map<std::string, std::string>& GetCookieMapRef()
    {
        return CookieMap;
    }

    std::optional<std::string> GetCookieValue(const std::string_view key) const
    {
        if (auto it{ CookieMap.find(key.data()) }; it != CookieMap.end())
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
