#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <map>
#include "common/Logger.hpp"
#include <fmt/ostream.h>

namespace
{
struct ComplicatedStruct
{
    std::string s {};
    int i{73};
    float f{3.1415f};
    friend std::ostream& operator<<(std::ostream& out, const ComplicatedStruct& src) {
        return out << fmt::format("{{s: \"{}\", i: {}, f: {}}}", src.s, src.i, src.f);
    };
};

const std::vector<std::string> expected_vals = {"val one", "val two"};
const std::vector<std::string_view> expected_keys = {"key one", "key two"};
const std::map<std::string_view, ComplicatedStruct> map = {{"key one", {"val one"}}, {"key two", {"val two"}}};
} // namespace
template <> struct fmt::formatter<ComplicatedStruct> : ostream_formatter {};

TEST(logger, printVals)
{
    EXPECT_NO_THROW(log_dbg("log vec: {}", expected_vals));
    EXPECT_NO_THROW(log_dbg("log vec: {}", map));
}
