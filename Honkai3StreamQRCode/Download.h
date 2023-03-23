#pragma once
#include "main.h"
class Download
{
public:
	Download();
	void curlDownlod(std::string url);
private:
	std::thread input_thread;
	bool isExit = false;
private:
	void check_input();
	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
};