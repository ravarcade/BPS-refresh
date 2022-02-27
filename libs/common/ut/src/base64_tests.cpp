#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "common/Logger.hpp"
#include "tools/base64.hpp"

namespace
{
const std::string text = "Lorem ipsum bla bla bla";
const std::string encoded_text = "TG9yZW0gaXBzdW0gYmxhIGJsYSBibGE=";
const std::string encoded_file = "Qk1mAAAAAAAAADYAAAAoAAAABAAAAAQAAAABABgAAAAAAAAAAADEDgAAxA4AAAAAAAAAAAAA/wBI/wCy3AD/"
                                 "bgD/kP8A//8A/5QA/yYAANj/AP+2AP9MIf8AAAAAQEBAAAD/AGr/";
} // namespace

TEST(base64, encode)
{
    log_trace("no to wtopa!!!");
    log_dbg("no to wtopa!!!");
    log_inf("no to wtopa!!!");
    log_err("no to wtopa!!!");
    EXPECT_THAT(tools::base64_encode(text), encoded_text);
}

TEST(base64, decodeAndEncodeAgain)
{
    auto decoded = tools::base64_decode(encoded_file);
    EXPECT_THAT(encoded_file, tools::base64_encode(decoded));
}