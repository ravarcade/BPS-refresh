#pragma once

#include <array>
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <string_view>
#include <thread>
#include "enumFormatter.hpp"

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

constexpr std::string_view removePath(const std::string_view path) noexcept
{
    size_t p = path.find_last_of("/\\");
    return std::string_view::npos != p ? path.substr(p + 1) : path;
}

template <const LogLevel logLevel, typename... Args>
void log(std::string_view file, int line, fmt::format_string<Args...> fmt, Args&&... args)
{
    if constexpr (static_cast<int>(logLevel) >= LOG_LEVEL)
    {
        auto now = std::chrono::system_clock::now();
        fmt::print(
            "{:%F %H:%M:%S}.{:06} [{}]/{} {}:{:d}: {}\n",
            now,
            (std::chrono::round<std::chrono::microseconds>(now).time_since_epoch() % std::chrono::seconds(1)).count(),
            fmt::streamed(std::this_thread::get_id()),
            logLevel,
            removePath(file),
            line,
            fmt::format(fmt, std::forward<Args>(args)...));
    }
}

#define FMT_TOSTRING(T)                                    \
    template <>                                            \
    struct fmt::formatter<T> : fmt::formatter<string_view> \
    {                                                      \
        template <typename FormatContext>                  \
        auto format(T, FormatContext&);                    \
    }

#define log_trace(...) log<LogLevel::TRACE>(__FILE__, __LINE__, __VA_ARGS__)
#define log_dbg(...) log<LogLevel::DEBUG>(__FILE__, __LINE__, __VA_ARGS__)
#define log_inf(...) log<LogLevel::INFO>(__FILE__, __LINE__, __VA_ARGS__)
#define log_wrn(...) log<LogLevel::WARNING>(__FILE__, __LINE__, __VA_ARGS__)
#define log_err(...) log<LogLevel::ERROR>(__FILE__, __LINE__, __VA_ARGS__)