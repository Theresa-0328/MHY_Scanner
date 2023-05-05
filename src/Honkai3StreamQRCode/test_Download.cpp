#include "Download.h"

Download::Download()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

Download::~Download()
{

}

size_t Download::write_data(void* ptr, size_t size, size_t nmemb, void* stream)// 定义回调函数，将curl下载的数据写入缓冲区
{
    Download* pThis = (Download*)stream;
    DWORD bytes_written = 0;
    if (pThis)
    {
        if (pThis->m_ExitThread)
        {
            return 0;
        }
        BOOL result = WriteFile((HANDLE)stream, ptr, (DWORD)(size * nmemb), &bytes_written, NULL);
        return bytes_written / size;
    }
}

void Download::stopDownload()
{
    //std::this_thread::sleep_for(std::chrono::seconds(3)); // 停止下载任务的等待时间
    //curl_easy_pause(curl, CURLPAUSE_ALL);
    //scurl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    //std::this_thread::sleep_for(std::chrono::milliseconds(300));
    //curl_easy_cleanup(curl);
    //curl_global_cleanup();
    m_ExitThread = true;
    std::cout << "下载已停止" << std::endl;
}

void Download::curlDownlod(std::string url)
{
    // 使用curl下载直播流
    if (curl)
    {
        //占用问题
        fp = CreateFile(
            L"./cache/output.flv",            // 文件名
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
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41");
            curl_easy_setopt(curl, CURLOPT_REFERER, "https://live.bilibili.com");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            //指定ssl版本，防止SSL connect error
            curl_easy_setopt(curl, CURLOPT_SSLVERSION, 3);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30000L);
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
            // 暂停下载操作
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_pause(curl, CURLPAUSE_ALL);
            curl_easy_cleanup(curl);
            CloseHandle(fp);
        }
    }
#ifdef _DEBUG
    std::cout << "===== Downlod Exiting =====" << std::endl;
#endif
}
