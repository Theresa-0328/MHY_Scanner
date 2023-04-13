#include "Download.h"

Download::Download() /*:input_thread(&Download::check_input, this) */
{
    CURL* curl = curl_easy_init();
    stop_download.store(false);
}

size_t Download::write_data(void* ptr, size_t size, size_t nmemb, void* stream)// 定义回调函数，将curl下载的数据写入缓冲区
{
    DWORD bytes_written = 0;
    BOOL result = WriteFile((HANDLE)stream, ptr, (DWORD)(size * nmemb), &bytes_written, NULL);
    if (result)
    {
        return bytes_written / size;
    }
    else
    {
        return 0;
    }
}

void Download::stopDownloadAfterDelay()
{
    //std::this_thread::sleep_for(std::chrono::seconds(3)); // 停止下载任务的等待时间
    
    stop_download.store(true);
    curl_easy_pause(curl, CURLPAUSE_ALL);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    std::cout << "清理curl" << std::endl;
    curl_easy_cleanup(curl);
}

void Download::curlDownlod(std::string url)
{
    curl_global_init(CURL_GLOBAL_ALL);

    // 使用curl下载直播流
    if (curl)
    {
        //fopen_s下载时打不开
        HANDLE fp = CreateFile(
            L".\\cache\\output.flv",            // 文件名
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
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30000L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

            //std::thread stop_thread(&Download::stopDownloadAfterDelay, this);
            //std::this_thread::sleep_for(std::chrono::seconds(1));
            CURLcode res = curl_easy_perform(curl);
            //res = CURLE_OK;
            std::cout << "++++++++++++++++++++++++++++++++" << std::endl;
            if (res == CURLE_OK)
            {
                std::cout << "Download completed successfully." << std::endl;
            }
            else
            {
                std::cout << "Error downloading: " << curl_easy_strerror(res) << std::endl;
            }
            //stop_thread.join();
            // 暂停下载操作
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_pause(curl, CURLPAUSE_ALL);
            // 关闭文件句柄并释放CURL对象
            CloseHandle(fp);
            //curl_easy_cleanup(curl);
        }
    }

#ifdef _DEBUG
    std::cout << "Exiting Down." << std::endl;
#endif
}
