#pragma once

#include <chrono>

inline size_t GetUnixTimeStampSeconds()
{
    return static_cast<size_t>(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count());
}