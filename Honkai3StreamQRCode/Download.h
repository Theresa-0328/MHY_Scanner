#pragma once
#include "main.h"
class Download
{
public:
	bool isExit = false;
public:
	Download();
	void curlDownlod(std::string url);
	void check_input();
private:
	std::thread input_thread;
private:
	static size_t write_data(void* ptr, size_t size, size_t nmemb, void* stream);
};