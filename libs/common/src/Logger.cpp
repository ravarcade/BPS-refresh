#include "common/Logger.hpp"
#include <cassert>

// template<typename ParseContext>
// constexpr auto fmt::formatter<LogLevel>::parse(ParseContext& ctx)
// {
//   return ctx.begin();
// }

// template<typename FormatContext>
// auto fmt::formatter<LogLevel>::format(LogLevel const& logLevel, FormatContext& ctx)
// {
//     static const std::array<const std::string_view,3>  levTxt {"TRACE", "DEBUG", "INFO"};
//     size_t logLevelIdx = static_cast<int>(logLevel);
//     assert(logLevelIdx >= 0 and logLevelIdx < levTxt.size());
//     return fmt::format_to(ctx.out(), levTxt.at(logLevelIdx));
// }

// std::ostream& operator <<(std::ostream& out, const LogLevel& logLevel)
// {
//   switch(logLevel)
//   {
//     case LogLevel::TRACE: out << "TRACE"; break;
//     case LogLevel::DEBUG: out << "DEBUG"; break;
//     case LogLevel::INFO: out << "INFO"; break;
//   }
//   return out;
// }

// std::ostream& operator <<(std::ostream& out, LogLevel logLevel)
// {
//   switch(logLevel)
//   {
//     case LogLevel::TRACE: out << "TRACE"; break;
//     case LogLevel::DEBUG: out << "DEBUG"; break;
//     case LogLevel::INFO: out << "INFO"; break;
//   }
//   return out;
// }
