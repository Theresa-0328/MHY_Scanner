#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <memory>

#include "ApiDefs.hpp"

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

class LiveBili
{
public:
    explicit LiveBili(const std::string& roomID);
    LiveStreamInfo GetLiveStreamInfo();

private:
    std::string GetLinkByRealRoomID(const std::string& realRoomID);
    std::string GetStreamUrl(const cpr::Parameters param);

    std::string roomID;
    std::string realRoomID;
};

class LiveDouyin
{
public:
    explicit LiveDouyin(const std::string& roomID);
    LiveStreamInfo GetLiveStreamInfo();

private:
    std::string GetStreamLinkFromResponse(const nlohmann::json& data);
    std::string m_roomID;
};

template<typename T>
LiveStreamInfo GetLiveInfo(const std::string& roomID)
{
    return T(roomID).GetLiveStreamInfo();
}

LiveStreamInfo GetLiveInfo(const LivePlatform platform, const std::string& roomID);