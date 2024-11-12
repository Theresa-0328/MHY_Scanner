#pragma once

#include <format>
#include <source_location>
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdint>

#define TRRLOG_BEGIN_NAMESPACE \
    namespace trrlog           \
    {
#define TRRLOG_END_NAMESPACE }

#define DETAILS_BEGIN_NAMESPACE \
    namespace details           \
    {
#define DETAILS_END_NAMESPACE }

TRRLOG_BEGIN_NAMESPACE

#define TRRLOG_FOREACH_LOG_LEVEL(f) \
    f(trace)                        \
        f(debug)                    \
            f(info)                 \
                f(critica)          \
                    f(warn)         \
                        f(error)    \
                            f(fatal)

enum class LogLevel : std::uint8_t
{
#define _FUNCTION(name) name,
    TRRLOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
};

DETAILS_BEGIN_NAMESPACE

inline std::string log_level_name(LogLevel level)
{
    switch (level)
    {
#define _FUNCTION(name)  \
    case LogLevel::name: \
        return #name;
        TRRLOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    }
    return "unknown";
}

inline LogLevel log_level_from_name(std::string level)
{
#define _FUNCTION(name) \
    if (level == #name) \
        return LogLevel::name;
    TRRLOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION
    return LogLevel::info;
}

template<class T>
class with_source_location
{
public:
    template<class U>
        requires std::constructible_from<T, U>
    consteval with_source_location(U&& inner, std::source_location loc = std::source_location::current()) :
        inner(std::forward<U>(inner)), loc(std::move(loc))
    {
    }
    constexpr T const& format() const { return inner; }
    constexpr std::source_location const& location() const { return loc; }

private:
    T inner;
    std::source_location loc;
};

inline LogLevel g_max_level = []() -> LogLevel {
    char* value = nullptr;
    size_t size;
    if (getenv_s(&size, nullptr, 0, "TRRLOG_LEVEL"); size)
{
    return details::log_level_from_name(value);
}
return LogLevel::debug; }();

inline std::ofstream g_log_file{};

inline const std::chrono::time_zone* getTimeZone()
{
    const std::chrono::time_zone* tz{};
    try
    {
        tz = std::chrono::current_zone();
    }
    catch (...)
    {
        tz = std::chrono::locate_zone("Europe/London");
    }
    return tz;
}

inline void output_log(LogLevel level, std::string msg, std::source_location const& loc)
{
    //Memory leak?
    static const std::chrono::time_zone* tz{ getTimeZone() };
    std::chrono::zoned_time now{ tz, std::chrono::system_clock::now() };
    msg = std::format("{} {}:{} [{}] {}", now, loc.file_name(), loc.line(), details::log_level_name(level), msg);
    if (g_log_file)
    {
        g_log_file << msg + '\n';
    }
    if (level >= g_max_level)
    {
        std::cout << msg + '\n';
    }
}

DETAILS_END_NAMESPACE

inline void SetLogFile(std::string path)
{
    details::g_log_file = std::ofstream(path, std::ios::app);
}

inline void SetLogLevel(LogLevel level)
{
    details::g_max_level = level;
}

template<typename... Types>
void generic_log(LogLevel level, details::with_source_location<std::format_string<Types...>> Fmt, Types&&... Args)
{
    auto const& loc = Fmt.location();
    auto msg = std::vformat(Fmt.format().get(), std::make_format_args(Args...));
    details::output_log(level, std::move(msg), loc);
    //std::cout << loc.file_name() << " : " << loc.line() << " [" << details::log_level_name(level) << "] " << std::vformat(Fmt.format().get(), std::make_format_args(Args...)) << std::endl;
}

#define _FUNCTION(name)                                                                               \
    template<typename... Types>                                                                       \
    void Log_##name(details::with_source_location<std::format_string<Types...>> Fmt, Types&&... Args) \
    {                                                                                                 \
        return generic_log(LogLevel::name, std::move(Fmt), std::forward<Types>(Args)...);             \
    }
TRRLOG_FOREACH_LOG_LEVEL(_FUNCTION)
#undef _FUNCTION

#define TRRLOG_P(x) ::trrlog::Log_debug(#x "={}", x)

TRRLOG_END_NAMESPACE

#undef TRRLOG_BEGIN_NAMESPACE
#undef TRRLOG_END_NAMESPACE
#undef DETAILS_BEGIN_NAMESPACE
#undef DETAILS_END_NAMESPACE