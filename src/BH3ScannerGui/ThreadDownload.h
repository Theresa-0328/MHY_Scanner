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
	static size_t DownloadCallback(void* ptr, size_t size, size_t nmemb, void* stream);
	void downloadInit(std::string url);
	void stopDownload();
	void restartDownload();
	void run();
private:
	std::string url;
	HANDLE fp = NULL;
	CURL* curl = curl_easy_init();
	bool m_ExitThread = false;
	QMutex m_mux;
};