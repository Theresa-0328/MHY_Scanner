#pragma once
#include "main.h"

class utils
{
public:
	std::string Url(std::string url, std::map<std::string, std::string> params);
	std::string GetRequest(std::string address, std::map<std::string, std::string> headers = {});
	static size_t req_reply(void* ptr, size_t size, size_t nmemb, void* stream);//静态成员函数
	std::string UTF8_To_string(const std::string& str);
	std::string string_To_UTF8(const std::string& str);
private:
};