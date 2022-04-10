#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <map>
#include "tools/to_vector.hpp"

namespace
{
struct ComplicatedStruct
{
    std::string s {};
    int i{};
    float f{};
};
const std::vector<std::string> expected_vals = {"val one", "val two"};
const std::vector<std::string_view> expected_keys = {"key one", "key two"};
const std::map<std::string_view, ComplicatedStruct> map = {{"key one", {"val one"}}, {"key two", {"val two"}}};
} // namespace

TEST(to_vector, extractVals)
{
    auto extractValsFromComplicatedStruct = [](const auto& it) { return it.second.s; };
    EXPECT_THAT(tools::to_vector(map, extractValsFromComplicatedStruct), expected_vals);
}

TEST(to_vector, extractKeys)
{
    auto extractKeysFromComplicatedStruct = [](const auto& it) { return it.first; };
    EXPECT_THAT(tools::to_vector(map, extractKeysFromComplicatedStruct), expected_keys);
}
