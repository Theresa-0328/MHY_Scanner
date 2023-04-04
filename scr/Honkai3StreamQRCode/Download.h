#pragma once
#include<curl/curl.h>
#include <mutex>
#include <iostream>
class Download
{
public:
	Download();
	void curlDownlod(std::string url);
	void stopDownloadAfterDelay();
private:
	std::thread input_thread;
	CURL* curl = curl_easy_init();
private:
	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
	
	std::atomic<bool> stop_download;
	std::mutex mux;
	HANDLE fp;
};