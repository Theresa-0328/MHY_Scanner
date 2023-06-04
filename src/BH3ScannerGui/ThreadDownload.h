#pragma once

#include <QThread>
#include <curl/curl.h>
#include <iostream>
#include <QMutex>
class ThreadDownload  : public QThread
{
	Q_OBJECT
public:
	ThreadDownload(QObject *parent);
	~ThreadDownload();
	void downloadInit(std::string url);
	void stopDownload();
	void run();
private:
	static size_t DownloadCallback(void* ptr, size_t size, size_t nmemb, void* vThisPtr);
	std::string url;
	HANDLE fp = NULL;
	CURL* curl = curl_easy_init();
	bool m_ExitThread = false;
	QMutex m_mux;
};