#include "ThreadDownload.h"

ThreadDownload::ThreadDownload()
	: QThread()
{
	curl_global_init(CURL_GLOBAL_ALL);
}

ThreadDownload::~ThreadDownload()
{
	CloseHandle(fp);
	this->wait();
	curl_easy_cleanup(curl);
}

size_t ThreadDownload::DownloadCallback(void* ptr, size_t size, size_t nmemb, void* vThisPtr)
{
	ThreadDownload* pThis = (ThreadDownload*)vThisPtr;
	QMutexLocker lock(&pThis->m_mux);
	DWORD bytes_written = 0;
	if (pThis->m_ExitThread)
	{
		return 0;
	}
	if (pThis)
	{
		BOOL result = WriteFile(pThis->fp, ptr, (DWORD)(size * nmemb), &bytes_written, NULL);
	}
	return bytes_written / size;
}

void ThreadDownload::stop()
{
	QMutexLocker lock(&m_mux);
	m_ExitThread = true;
#ifdef _DEBUG
	std::cout << "send stop download" << std::endl;
#endif // DEBUG
}

void ThreadDownload::downloadInit(const std::string& url)
{
	this->url = url;
}

void ThreadDownload::run()
{
	m_ExitThread = false;
	if (!curl)
	{
		return;
	}
	fp = CreateFile(
		L".\\Cache\\output.flv",            // 文件名
		GENERIC_WRITE,                      // 访问权限
		FILE_SHARE_READ | FILE_SHARE_WRITE, // 共享模式
		NULL,                               // 安全属性
		CREATE_ALWAYS,                      // 打开模式
		FILE_ATTRIBUTE_NORMAL,              // 文件属性
		NULL                                // 模板文件句柄
	);
	if (fp)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) \
			Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41");
		curl_easy_setopt(curl, CURLOPT_REFERER, "https://live.bilibili.com");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, DownloadCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)this);
		//指定ssl版本，否则可能出现SSL connect error
		curl_easy_setopt(curl, CURLOPT_SSLVERSION, 3);
		//curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30000L);
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

		CURLcode res = curl_easy_perform(curl);
		if (res == CURLE_OK)
		{
			std::cout << "Download completed successfully." << std::endl;
		}
		else
		{
			std::cout << "Error downloading: " << curl_easy_strerror(res) << std::endl;
		}
	}
#ifdef _DEBUG
	std::cout << "===== Downlod Exiting =====" << std::endl;
#endif
}