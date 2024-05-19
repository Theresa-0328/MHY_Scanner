#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include <Windows.h>

inline std::string urlEncode(const std::string& str)
{
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto itr = str.cbegin(), end = str.cend(); itr != end; ++itr)
    {
        const unsigned char c = *itr;

        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~' || c == '!' || c == '*' ||
            c == '\'' || c == '(' || c == ')' || c == ';' || c == ':' || c == '@' || c == '&' ||
            c == '=' || c == '$' || c == ',' || c == '/' || c == '?' || c == '#' || c == '[' || c == ']')
        {
            escaped << c;
            continue;
        }

        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char)c);
        escaped << std::nouppercase;
    }
    return escaped.str();
}

inline std::string urlDecode(const std::string& str)
{
    std::string str_decode;
    int i;
    char* cd = (char*)str.c_str();
    char p[2];
    for (i = 0; i < strlen(cd); i++)
    {
        memset(p, '\0', 2);
        if (cd[i] != '%')
        {
            str_decode += cd[i];
            continue;
        }
        p[0] = cd[++i];
        p[1] = cd[++i];
        p[0] = p[0] - 48 - ((p[0] >= 'A') ? 7 : 0) - ((p[0] >= 'a') ? 32 : 0);
        p[1] = p[1] - 48 - ((p[1] >= 'A') ? 7 : 0) - ((p[1] >= 'a') ? 32 : 0);
        str_decode += (unsigned char)(p[0] * 16 + p[1]);
    }
    return str_decode;
}

inline std::string replaceQuotes(const std::string& str)
{
    std::string newStr;
    for (int i = 0; i < str.length(); i++)
    {
        if (str[i] == '\"')
        {
            newStr += '\\';
        }
        newStr += str[i];
    }
    return newStr;
}

inline std::string string_To_UTF8(const std::string& str)
{
    int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
    wchar_t* pwBuf = new wchar_t[nwLen + 1]; //一定要加1，不然会出现尾巴
    memset(pwBuf, 0, (nwLen + 1) * sizeof(wchar_t));

    ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), static_cast<int>(str.length()), pwBuf, nwLen);

    int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete[] pwBuf;
    delete[] pBuf;

    return retStr;
}

inline std::string UTF8_To_string(const std::string& str)
{
    int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    wchar_t* pwBuf = new wchar_t[nwLen + 1];
    memset(pwBuf, 0, (nwLen + 1) * sizeof(wchar_t));

    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), static_cast<int>(str.length()), pwBuf, nwLen);

    int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, 0, NULL, NULL);

    char* pBuf = new char[nLen + 1];
    memset(pBuf, 0, nLen + 1);

    WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

    std::string retStr(pBuf);

    delete[] pBuf;
    delete[] pwBuf;

    return retStr;
}

inline std::string escapeString(const std::string& input)
{
    std::string escaped;
    for (char c : input)
    {
        if (c == '\\')
        {
            escaped += "\\\\";
        }
        else if (c == '\"')
        {
            escaped += "\\\"";
        }
        else if (c == '\n')
        {
            escaped += "\\n";
        }
        else if (c == '\t')
        {
            escaped += "\\t";
        }
        else if (c == '\r')
        {
            escaped += "\\r";
        }
        else
        {
            escaped += c;
        }
    }
    return escaped;
}

inline std::string unescapeString(const std::string& input)
{
    std::string unescaped;
    bool isEscape = false;
    for (char c : input)
    {
        if (isEscape)
        {
            if (c == 'n')
            {
                unescaped += '\n';
            }
            else if (c == 't')
            {
                unescaped += '\t';
            }
            else if (c == 'r')
            {
                unescaped += '\r';
            }
            else
            {
                unescaped += c;
            }
            isEscape = false;
        }
        else
        {
            if (c == '\\')
            {
                isEscape = true;
            }
            else
            {
                unescaped += c;
            }
        }
    }
    return unescaped;
}

//TODO temp
inline void replace0026WithAmpersand(std::string& str)
{
    std::string searchString = "0026";
    std::string replacement = "&";
    size_t pos = 0;

    while ((pos = str.find(searchString, pos)) != std::string::npos)
    {
        str.replace(pos, searchString.length(), replacement);
        pos += replacement.length();
    }
}