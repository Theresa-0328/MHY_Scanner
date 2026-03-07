#include "LiveStreamLink.h"

#include <format>
#include <fstream>
#include <regex>
#include <random>
#include <chrono>

#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

LiveBili::LiveBili(const std::string& roomID) :
    roomID(roomID)
{
}

LiveStreamInfo LiveBili::GetLiveStreamInfo()
{
    // 获取房间初始化信息
    auto r = cpr::Get(cpr::Url{ std::format("{}?id={}", live_bili_room_init, roomID) });
    if (r.error || r.status_code != 200 || r.text.empty())
    {
        return { LiveStreamStatus::Error, "" };
    }
    try
    {
        auto roomInfo = nlohmann::json::parse(r.text, nullptr, false);
        if (roomInfo.is_discarded())
        {
            return { LiveStreamStatus::Error, "" };
        }
        int code = roomInfo["code"].get<int>();
        if (code == 60004)
        {
            return { LiveStreamStatus::Absent, "" };
        }
        if (code != 0)
        {
            return { LiveStreamStatus::Error, "" };
        }
        const auto& data = roomInfo["data"];
        int liveStatus = data["live_status"].get<int>();
        if (liveStatus != 1)
        {
            return { LiveStreamStatus::NotLive, "" };
        }
        // 更新真实房间ID
        if (data.contains("room_id"))
        {
            realRoomID = std::to_string(data["room_id"].get<int>());
        }

        std::string link = GetLinkByRealRoomID(realRoomID);
        if (link.empty())
        {
            return { LiveStreamStatus::Error, "" };
        }
        return { LiveStreamStatus::Normal, link };
    }
    catch (const nlohmann::json::exception& e)
    {
        return { LiveStreamStatus::Error, "" };
    }
}

std::string LiveBili::GetLinkByRealRoomID(const std::string& realRoomID)
{
    const cpr::Parameters params = {
#if 0
        appkey:iVGUTjsxvpLeuDCf
        build:6215200
        c_locale:zh_CN
#endif
        { "codec", "0" },
#if 0
        device:web
        device_name:VTR-AL00
        dolby:1
#endif
        { "format", "0,2" },
#if 0
        free_type:0
        http:1
        mask:0
        mobi_app:web
        network:wifi
        no_playurl:0
#endif
        { "only_audio", "0" },
        { "only_video", "0" },
#if 0
        //TODO platform 会影响下载时使用的referer
        {"platform", "h5" },
        play_type:0
#endif
        { "protocol", "0,1" },
        { "qn", "10000" },
        { "room_id", realRoomID },
#if 0
        s_locale:zh_CN
        statistics:{\"appId\":1,\"platform\":3,\"version\":\"6.21.5\",\"abtest\":\"\"}
#endif
    };
    return GetStreamUrl(live_bili_v2_PlayInfo, params);
}

std::string LiveBili::GetStreamUrl(const std::string& url, const cpr::Parameters param)
{
    auto r = cpr::Get(cpr::Url{ url }, param);
    if (r.error || r.status_code != 200 || r.text.empty())
    {
        return "";
    }
    try
    {
        auto r = cpr::Get(cpr::Url{ url }, param);
        if (r.error || r.status_code != 200 || r.text.empty())
        {
            return "";
        }
        auto playInfo = nlohmann::json::parse(r.text, nullptr, false);
        if (playInfo.is_discarded())
        {
            return "";
        }
        const auto& data = playInfo["data"];
        const auto& playurl_info = data["playurl_info"];
        const auto& playurl = playurl_info["playurl"];
        const auto& stream = playurl["stream"][0];
        const auto& format = stream["format"][0];
        const auto& codec = format["codec"][0];

        std::string base_url = codec["base_url"].get<std::string>();
        std::string extra = codec["url_info"][0]["extra"].get<std::string>();
        std::string host = codec["url_info"][0]["host"].get<std::string>();

        return host + base_url + extra;
    }
    catch (const nlohmann::json::exception& e)
    {
        return "";
    }
}

LiveDouyin::LiveDouyin(const std::string& roomID) :
    m_roomID(roomID)
{
}

LiveStreamInfo LiveDouyin::GetLiveStreamInfo()
{
    try
    {
        // 构建请求参数
        std::string user_agent =
            "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 "
            "(KHTML, like Gecko) Chrome/92.0.4515.159 Safari/537.36";

        const std::map<std::string, std::string> headers = {
            { "User-Agent", user_agent },
            { "referer", "https://live.douyin.com/" },
            { "cookie", "enter_pc_once=1; UIFID_TEMP=29a1f63ec682dc0a0df227dd163e2b46e3a6390e403335fa4c2c6d1dc0ec5ffa7a288170e8828ecb8b2f0f16b3219daa18ad5d7faf7fb5fbb64df454c3b471cc1db9c0b5eb2cbc8e0cb1e690f5c1fbd6; stream_recommend_feed_params=%22%7B%5C%22cookie_enabled%5C%22%3Atrue%2C%5C%22screen_width%5C%22%3A2560%2C%5C%22screen_height%5C%22%3A1440%2C%5C%22browser_online%5C%22%3Atrue%2C%5C%22cpu_core_num%5C%22%3A16%2C%5C%22device_memory%5C%22%3A8%2C%5C%22downlink%5C%22%3A10%2C%5C%22effective_type%5C%22%3A%5C%224g%5C%22%2C%5C%22round_trip_time%5C%22%3A50%7D%22; hevc_supported=true; odin_tt=363047b47492a2e153d67e7022684ffd83726a0c57322991e6650da1dbe2fc0adb471e8be38efa85bf0ab9788a8e237d481c8fc488ef859f4476fc6ffd50dd31a258add2954b3fcf03cd546357df6a53; strategyABtestKey=%221772897157.15%22; passport_csrf_token=d71952d93315e4df5cc8373e4cdc2447; passport_csrf_token_default=d71952d93315e4df5cc8373e4cdc2447; home_can_add_dy_2_desktop=%221%22; biz_trace_id=fab9d888; ttwid=1%7CP0feYUzzIsbXr2aaLLBWHYtwVD4-6CV2voO9bAUQ7PU%7C1772897161%7Cd72bed8f6f576a1dfb7b8d1032c76706ce93b3ba3ac5b21e79501db1c2f17c9f; __security_mc_1_s_sdk_crypt_sdk=0ef27763-40a0-b3c3; bd_ticket_guard_client_data=eyJiZC10aWNrZXQtZ3VhcmQtdmVyc2lvbiI6MiwiYmQtdGlja2V0LWd1YXJkLWl0ZXJhdGlvbi12ZXJzaW9uIjoxLCJiZC10aWNrZXQtZ3VhcmQtcmVlLXB1YmxpYy1rZXkiOiJCSjhja053TW16SWxIVWQzazF4d2F6bXdQdm1JZjUrcElEVWR2MmpTN3czVWRKRWZ6djBIN1g5Z3dINUNnRkpSSGIzOEFvWTZYSEZsOEdWcGd1dmN4OGc9IiwiYmQtdGlja2V0LWd1YXJkLXdlYi12ZXJzaW9uIjoyfQ%3D%3D; bd_ticket_guard_client_web_domain=2; bd_ticket_guard_client_data_v2=eyJyZWVfcHVibGljX2tleSI6IkJKOGNrTndNbXpJbEhVZDNrMXh3YXptd1B2bUlmNStwSURVZHYyalM3dzNVZEpFZnp2MEg3WDlnd0g1Q2dGSlJIYjM4QW9ZNlhIRmw4R1ZwZ3V2Y3g4Zz0iLCJyZXFfY29udGVudCI6InNlY190cyIsInJlcV9zaWduIjoiNkxSc0hxbFZ4bUhHSFVzMCtsQ0dLaGNlU242bVZxZzRRRFJmdjJ1RzZCaz0iLCJzZWNfdHMiOiIjaUZua3E0M0pNV25FWGlQNW15b3grVTlWdUNrL3B4ZnQveVlsL3o5eWdpWnRSOUZjbEZSSmFGOXk1T1lWIn0%3D; sdk_source_info=7e276470716a68645a606960273f276364697660272927676c715a6d6069756077273f276364697660272927666d776a68605a607d71606b766c6a6b5a7666776c7571273f275e58272927666a6b766a69605a696c6061273f27636469766027292762696a6764695a7364776c6467696076273f275e582729277672715a646971273f2763646976602729277f6b5a666475273f2763646976602729276d6a6e5a6b6a716c273f2763646976602729276c6b6f5a7f6367273f27636469766027292771273f27343636343334323c3d37323234272927676c715a75776a716a666a69273f2763646976602778; bit_env=7NCrRegY020LGVG5Yx8HFRWB73RARpFbj-iyQ1LwqU0cDI9moZj9ecPpsbpkSaMTEyZqsilIKiI_lt70BB_G6Dod7wN8rkLhE631Bz9wC_ixgEAlNeIdElXvK3C9gool9MEa3Y5xuHt4r36Y7HkF5YAELvmsxcB8412Lfy3XuXNgybsvbLqhJrUhs-rG5nU1V-xyc70ffKH2TqV_ZxyfiI1Qn7a3LENvJkf8V9ntSbLM3qoKcG5so8A6lMQ5LoyEsZgIq4i-rMHEO1Bc13y9wvk3oi-sJI76Ez-qeR_ArnBjdI6ZLTG_MUWfLeu9Ikz79n1nYgUl8r6sEXw3L3au4iOY5cfKhxFNEOszmGtoiAE8n91LvALTHWW_yZgi93E_ne4h-gOaqKLccAN05tCphxDc1uAoS3i4jBcKdnyF6ZVyGuJ_FSi4NQFvVGupfejzLbrfZoWDfGj6pgZpGEMCHnF0w_ajPy3jko_TKwdpi7DW6q49w-fjUYSjc3vJ137yj0N3um5dVKvIFJM1v0yBsavXNheto_S1GKCVq-6LTcM%3D; gulu_source_res=eyJwX2luIjoiOWYxNmJiYTEwNTIwMTgyMzIwOGMyZWYyYzllN2RkYWE1YjRjNTgzYmI0ZDhkYzAwNWNlODQxZjgwNTU3MzA5ZCJ9; passport_auth_mix_state=nt3zeeuup2eyy8cn750jdgpj52a9ldxlw3vzw45ba2eu8j77; is_dash_user=1; x-web-secsdk-uid=17063330-58d4-4719-9971-dba52fc661ab; __live_version__=%221.1.4.9549%22; has_avx2=null; device_web_cpu_core=16; device_web_memory_size=8; webcast_local_quality=null; live_use_vvc=%22false%22; csrf_session_id=5fe8f9d1180e55817920dae0808993ba; live_debug_info=%7B%22roomId%22%3A%227614515520083118863%22%2C%22resolution%22%3A%7B%22width%22%3A1920%2C%22height%22%3A1080%7D%2C%22fps%22%3A70%2C%22audioDataRate%22%3A48000%2C%22droppedFrames%22%3A4%2C%22totalFrames%22%3A65%2C%22videoBuffer%22%3A%5B%5D%2C%22src%22%3A%22https%3A%2F%2Fpull-flv-q13.douyincdn.com%2Fthirdgame%2Fstream-695437557938520894.flv%3Farch_hrchy%3Dh1%26exp_hrchy%3Dh1%26expire%3D1773502023%26major_anchor_level%3Dcommon%26sign%3D5d4807ba64265f674729e812ec33618c%26t_id%3D037-202603072327037BDC1553A514D5F37F8C-QZsvPZ%26unique_id%3Dstream-695437557938520894_830_flv%26_session_id%3D037-202603072327037BDC1553A514D5F37F8C-QZsvPZ.1772897224232.33396%26rsi%3D1%26abr_pts%3D-800%22%2C%22linkmicInfo%22%3A%7B%22uiLayout%22%3A0%2C%22playModes%22%3A%5B%5D%2C%22allDevices%22%3A%22%E8%BF%9E%E7%BA%BF%E8%AE%BE%E5%A4%87%EF%BC%9A%E7%94%B3%E8%AF%B7%E8%BF%9E%E7%BA%BF%E5%90%8E%E6%89%8D%E8%8E%B7%E5%8F%96%22%2C%22audioInputs%22%3A%5B%5D%2C%22videoInputs%22%3A%5B%5D%7D%2C%22href%22%3A%22https%3A%2F%2Flive.douyin.com%2F262229562462%3Fanchor_id%3D60708713854%26follow_status%3D0%26is_vs%3D0%26vs_ep_group_id%3D%26vs_episode_id%3D%26vs_episode_stage%3D%26vs_season_id%3D%22%7D; fpk1=U2FsdGVkX19Xphctu6x8/IFxEj3mGvQobR7U2Gy90RThMds9G7h1ZgbvhsMLPFfJL+8+eZ5CzEghbCVjENUCnA==; fpk2=800cce95768a9a4605cb3f6b181e9057; h265ErrorNum=-1; webcast_leading_last_show_time=1772897235315; webcast_leading_total_show_times=1; IsDouyinActive=false; live_can_add_dy_2_desktop=%220%22" }
        };
        std::string params =
            "aid=6383&app_name=douyin_web&live_id=1&device_platform=web&"
            "browser_language=zh-CN&browser_platform=Win32&browser_name=Edge&"
            "browser_version=139.0.0.0&is_need_double_stream=false&web_rid=" +
            m_roomID;
        std::string url = std::string(live_douyin_room) + params;
        auto response = cpr::Get(
            cpr::Url{ url },
            cpr::Header{ headers.begin(), headers.end() });
        if (response.error || response.status_code != 200 || response.text.empty())
        {
            return { LiveStreamStatus::Error, "" };
        }
        auto streamInfo = nlohmann::json::parse(response.text);
        int status_code = streamInfo["status_code"].get<int>();
        if (status_code != 0)
        {
            return { LiveStreamStatus::Absent, "" };
        }
        // 获取直播数据
        const auto& data = streamInfo["data"]["data"][0];
        int status = data["status"].get<int>();
        // 抖音 status == 2 代表是开播的状态
        if (status == 2)
        {
            std::string link = GetStreamLinkFromResponse(data);
            if (link.empty())
            {
                return { LiveStreamStatus::Error, "" };
            }
            return { LiveStreamStatus::Normal, link };
        }
        // status == 4 代表未开播
        else if (status == 4)
        {
            return { LiveStreamStatus::NotLive, "" };
        }
        return { LiveStreamStatus::Error, "" };
    }
    catch (...)
    {
        return { LiveStreamStatus::Error, "" };
    }
}

std::string LiveDouyin::GetStreamLinkFromResponse(const nlohmann::json& data)
{
    try
    {
        const auto& stream_url = data["stream_url"];

        if (stream_url.contains("pull_datas"))
        {
            const auto& pullDatas = stream_url["pull_datas"];
            if (!pullDatas.empty())
            {
                auto doubleScreenStreams = pullDatas.begin().value();
                std::string stream_data_str = doubleScreenStreams["stream_data"].get<std::string>();
                auto streamData = nlohmann::json::parse(stream_data_str);

                return streamData["data"]["origin"]["main"]["flv"].get<std::string>();
            }
        }
        if (stream_url.contains("live_core_sdk_data"))
        {
            std::string stream_data_str =
                stream_url["live_core_sdk_data"]["pull_data"]["stream_data"].get<std::string>();
            auto streamData = nlohmann::json::parse(stream_data_str);

            return streamData["data"]["origin"]["main"]["flv"].get<std::string>();
        }

        return "";
    }
    catch (...)
    {
        return "";
    }
}

std::unique_ptr<ILiveStreamProvider> CreateLiveProvider(LivePlatform platform, const std::string& roomID)
{
    switch (platform)
    {
    case LivePlatform::Douyin:
        return std::make_unique<LiveDouyin>(roomID);
    case LivePlatform::BiliBili:
        return std::make_unique<LiveBili>(roomID);
    default:
        assert(false && "Unknown live platform");
        return nullptr;
    }
}