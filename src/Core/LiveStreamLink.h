#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <memory>

#include "Common.h"

namespace cpr
{
class Parameters;
};

enum class LiveStreamStatus : uint8_t
{
    Normal = 0,
    Absent = 1,
    NotLive = 2,
    Error = 3
};

enum class LivePlatform
{
    Douyin = 0,
    BiliBili = 1
};

struct LiveStreamInfo
{
    LiveStreamStatus status;
    std::string link;
};

class ILiveStreamProvider
{
public:
    virtual ~ILiveStreamProvider() = default;
    virtual LiveStreamInfo GetLiveStreamInfo() = 0;
};

class LiveBili : public ILiveStreamProvider
{
public:
    explicit LiveBili(const std::string& roomID);
    LiveStreamInfo GetLiveStreamInfo() override;

private:
    std::string GetLinkByRealRoomID(const std::string& realRoomID);
    std::string GetStreamUrl(const std::string& url, const cpr::Parameters param);

    std::string roomID;
    std::string realRoomID;
};

class LiveDouyin : public ILiveStreamProvider
{
public:
    explicit LiveDouyin(const std::string& roomID);
    LiveStreamInfo GetLiveStreamInfo() override;

private:
    std::string GetStreamLinkFromResponse(const nlohmann::json& data);
    std::string m_roomID;
    static constexpr std::string_view live_douyin_room = "https://live.douyin.com/webcast/room/web/enter/?";
};

std::unique_ptr<ILiveStreamProvider> CreateLiveProvider(LivePlatform platform, const std::string& roomID);