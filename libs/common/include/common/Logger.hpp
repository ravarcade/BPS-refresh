#pragma once

#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <source_location>
#include <string_view>
#include <thread>

enum class LogLevel
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARNING = 3,
    ERROR = 4
};

template <>
struct fmt::formatter<LogLevel>
{
    template <typename ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <typename FormatContext>
    auto format(const LogLevel& logLevel, FormatContext& ctx)
    {
        static const std::array<const std::string_view, 5> levTxt{"TRC", "DBG", "INF", "WRN", "ERR"};
        return fmt::format_to(ctx.out(), "{0}", levTxt.at(static_cast<size_t>(logLevel)));
    }
};

constexpr std::string_view stripPath(const std::string_view path)
{
    size_t p = path.find_last_of("/\\");
    return std::string_view::npos != p ? path.substr(p + 1) : path;
}

template <const LogLevel logLevel, typename... T>
inline void log(const std::source_location location, std::string_view fmt, T&&... args)
{
    if constexpr (static_cast<int>(logLevel) >= LOG_LEVEL)
    {
        fmt::print("{:%S}", std::chrono::system_clock::now().time_since_epoch());
        // auto a = std::chrono::system_clock::now();
        // fmt::print("{:%FT%T%Oz}\n", a);
        // fmt::print("{}:{:d}", stripPath(location.file_name()), location.line());
        const auto& vargs = fmt::make_format_args(args...);
        fmt::print(
            "[{}]/{} {}:{:d}: {}\n",
            std::this_thread::get_id(),
            logLevel,
            stripPath(location.file_name()),
            location.line(),
            fmt::vformat(fmt, vargs));
    }
}

template <typename... Args>
void log_trace(
    std::string_view fmt,
    Args&&... args,
    const std::source_location location = std::source_location::current())
{
    log<LogLevel::TRACE>(location, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_dbg(
    std::string_view fmt,
    Args&&... args,
    const std::source_location location = std::source_location::current())
{
    log<LogLevel::DEBUG>(location, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_inf(
    std::string_view fmt,
    Args&&... args,
    const std::source_location location = std::source_location::current())
{
    log<LogLevel::INFO>(location, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_wrn(
    std::string_view fmt,
    Args&&... args,
    const std::source_location location = std::source_location::current())
{
    log<LogLevel::WARNING>(location, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
void log_err(
    std::string_view fmt,
    Args&&... args,
    const std::source_location location = std::source_location::current())
{
    log<LogLevel::ERROR>(location, fmt, std::forward<Args>(args)...);
}
