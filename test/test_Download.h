#pragma once
#include <curl/curl.h>
#include <iostream>
class Download
{
public:
	Download();
	~Download();
	void curlDownlod(std::string url);
	void stopDownload();
private:
	CURL* curl = curl_easy_init();
	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
	HANDLE fp = NULL;
	bool m_ExitThread = false;
};