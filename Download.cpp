#include "main.h"

std::atomic<bool> stop_download(false);
// 定义回调函数，将curl下载的数据写入缓冲区

Download::Download() :input_thread(&Download::check_input, this) 
{
    a = 1;
}

size_t Download::write_data(void* ptr, size_t size, size_t nmemb, void* stream)
{
    if (stop_download.load())
    {
        return 0;//表示不再写入数据
    }
    DWORD bytes_written = 0;
    BOOL result = WriteFile((HANDLE)stream, ptr, size * nmemb, &bytes_written, NULL);
    if (result == TRUE)
    {
        return bytes_written / size;
    }
    else
    {
        return 0;
    }
}

void Download::check_input()
{
    char ch = 0;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    while (a)
    {
        if (a)
        {
            ch = std::getchar();
        }
        if (ch == 's')
        {
            stop_download = true;
            break;
        }
    }
    stop_download = true;
}

void Download::curlDownlod(std::string url)
{
    curl_global_init(CURL_GLOBAL_ALL);

    // 使用curl下载直播流
    CURL* curl = curl_easy_init();
    if (curl)
    {
        //fopen_s下载时打不开
        HANDLE fp = CreateFile(
            L".\\cache\\output.flv",                  // 文件名
            GENERIC_WRITE,                  // 访问权限
            FILE_SHARE_READ | FILE_SHARE_WRITE, // 共享模式
            NULL,                           // 安全属性
            CREATE_ALWAYS,                  // 打开模式
            FILE_ATTRIBUTE_NORMAL,          // 文件属性
            NULL                            // 模板文件句柄
        );
        if (fp)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "https://xy111x22x233x2xy.mcdn.bilivideo.cn:486/live-bvc/344022/live_50329118_9516950_bluray.flv?expires=1676714106&pt=web&deadline=1676714106&len=0&oi=614544756&platform=web&qn=10000&trid=1000567f19db76774f46ae67776dda396354&uipk=100&uipv=100&nbs=1&uparams=cdn,deadline,len,oi,platform,qn,trid,uipk,uipv,nbs&cdn=cn-live-mcdn&upsig=4b9609ca985ee75207a9b5ef84cc775b&sk=4207df3de646838b084f14f252be3affacddc1131b55556afe312ac1012d0357&p2p_type=1&src=57345&sl=10&free_type=0&mid=0&sid=mcdn-wx-hnxx-cm-16000094&chash=0&sche=ban&score=18&pp=rtmp&machinezone=ylf&source=onetier&trace=2&site=2a6e8048d1102882bc97d0758815e06f&order=1");
            curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41");
            curl_easy_setopt(curl, CURLOPT_REFERER, "https://live.bilibili.com");
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30000L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);

            std::thread input_thread;

            CURLcode res = curl_easy_perform(curl);
            //res = CURLE_OK;
            if (res == CURLE_OK)
            {
                std::cout << "Download completed successfully." << std::endl;
                //stop_download.store(true);
                a = 0;

            }
            else
            {
                std::cout << "Error downloading: " << curl_easy_strerror(res) << std::endl;
                a = 0;
            }

            // 暂停下载操作
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
            curl_easy_pause(curl, CURLPAUSE_ALL);


            // 关闭文件句柄并释放CURL对象
            CloseHandle(fp);
            curl_easy_cleanup(curl);

            // 等待用户输入线程结束
            input_thread.join();
        }
    }

    std::cout << "Exiting program." << std::endl;
}
