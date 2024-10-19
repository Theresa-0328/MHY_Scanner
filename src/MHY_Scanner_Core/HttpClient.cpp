#include "HttpClient.h"

#include <iomanip>
#include <chrono>
#include <sstream>
#include <random>

HttpClient::HttpClient() :
    curl(nullptr)
{
    curl = curl_easy_init();
}

HttpClient::~HttpClient()
{
    curl_easy_cleanup(curl);
}

std::string HttpClient::MapToQueryString(const std::map<std::string, std::string>& params)
{
    std::ostringstream paramsTemp;
    bool first = true;
    for (const auto& kv : params)
    {
        if (!first)
        {
            paramsTemp << "&";
        }
        first = false;
        paramsTemp << kv.first << "=" << kv.second;
    }
    return paramsTemp.str();
}

std::map<std::string, std::string> HttpClient::QueryStringToMap(const std::string& queryString)
{
    std::map<std::string, std::string> params;

    size_t startPos = 0;
    size_t endPos;

    while (startPos < queryString.length())
    {
        endPos = queryString.find('&', startPos);
        if (endPos == std::string::npos)
        {
            endPos = queryString.length();
        }

        std::string param = queryString.substr(startPos, endPos - startPos);

        size_t equalPos = param.find('=');
        if (equalPos != std::string::npos)
        {
            std::string key = param.substr(0, equalPos);
            std::string value = param.substr(equalPos + 1);
            params[key] = value;
        }

        startPos = endPos + 1;
    }

    return params;
}

size_t HttpClient::req_reply(void* ptr, size_t size, size_t nmemb, void* stream) //回调
{
    //std::cout << "----->reply" << std::endl;
    std::string* str = (std::string*)stream;
    //std::cout << *str << std::endl;
    (*str).append((char*)ptr, size * nmemb);
    return size * nmemb;
}

bool HttpClient::GetRequest(std::string& response, const char* url, std::map<std::string, std::string> headers)
{
    CURLcode res{};
    if (!curl)
    {
        return false;
    }
    struct curl_slist* headerList = NULL;
    for (const auto& kv : headers)
    {
        std::string header = kv.first + ": " + kv.second;
        headerList = curl_slist_append(headerList, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, true);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HEADER, false);
    curl_easy_setopt(curl, CURLOPT_NOBODY, false);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res = curl_easy_perform(curl);

    curl_slist_free_all(headerList);
    if (res != CURLE_OK)
    {
        return false;
    }
    return true;
}

bool HttpClient::PostRequest(std::string& response, const char* url, const std::string& postParams, std::map<std::string, std::string> headers)
{
    CURLcode res{};
    if (!curl)
    {
        return false;
    }
    struct curl_slist* headerList = NULL;
    for (const auto& kv : headers)
    {
        std::string header = kv.first + ": " + kv.second;
        headerList = curl_slist_append(headerList, header.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    curl_easy_setopt(curl, CURLOPT_HEADER, 0);
    //设置请求为post请求
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postParams.c_str());
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, req_reply);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&response);
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

    res = curl_easy_perform(curl);

    curl_slist_free_all(headerList);
    if (res != CURLE_OK)
    {
        return false;
    }
    return true;
}

int HttpClient::getCurrentUnixTime() const
{
    return static_cast<int>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}