#pragma once

#include <string>
#include <map>
#include <iostream>

#include <curl/curl.h>

#include "Common.h"

class HttpClient
{
public:
    HttpClient();
    ~HttpClient();
    bool GetRequest(std::string& response, const char* url, std::map<std::string, std::string> headers = {});
    bool PostRequest(std::string& response, const char* url, const std::string& postParams, std::map<std::string, std::string> headers = {}, bool header = false);
    static std::string MapToQueryString(const std::map<std::string, std::string>& params);
    std::map<std::string, std::string> QueryStringToMap(const std::string& str);

private:
    static size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream);
    CURL* curl;
};