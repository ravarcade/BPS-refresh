#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "tools/contains.hpp"

namespace
{
const std::vector<std::string> stringVec = {"val one", "val two"};
const std::vector<std::string_view> stringViewVec = {"key one", "key two"};
} // namespace

TEST(contains, stringVec)
{
    EXPECT_TRUE(tools::contains(stringVec, "val two"));
    EXPECT_FALSE(tools::contains(stringVec, "val zero"));
    EXPECT_TRUE(tools::contains(stringViewVec, "key two"));
    EXPECT_FALSE(tools::contains(stringViewVec, "key zero"));
}
