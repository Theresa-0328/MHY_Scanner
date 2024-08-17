#include "LiveStreamLink.h"

void douyin_link()
{
    LiveDouyin l("");
    l.GetLiveStreamStatus();
    std::cout << l.GetLiveStreamLink() << std::endl;
}

void bilibili_link()
{
    LiveBili l("6");
    l.GetLiveStreamStatus();
    std::cout << l.GetLiveStreamLink() << std::endl;
}

void huya_link()
{
    LiveHuya l("660000");
    l.GetLiveStreamStatus();
    std::cout << l.GetLiveStreamLink() << std::endl;
}

int main()
{
    //douyin_link();
    //bilibili_link();
    //huya_link();
}
