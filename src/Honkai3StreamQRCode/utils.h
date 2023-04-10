﻿#pragma once
#include <string>
#include <map>
#include <curl/curl.h>
#include <iostream>
#include <chrono>
#include <sstream>
class utils
{
public:
	std::string Url(std::string url, std::map<std::string, std::string> params);
	//https://blog.csdn.net/weixin_44122235/article/details/128969128
	CURLcode GetRequest(std::string& response, std::string address, std::map<std::string, std::string> headers = {});
	CURLcode PostRequest(std::string& response, const std::string& url, const std::string& postParams, std::map<std::string, std::string> headers={});
	int getCurrentUnixTime();
	std::string urlEncode(const std::string& str);
	std::string unicodeEncode(const std::string& str);
	std::string UTF8_To_string(const std::string& str);
	std::string string_To_UTF8(const std::string& str);
private:
	static size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream);//静态成员函数
};