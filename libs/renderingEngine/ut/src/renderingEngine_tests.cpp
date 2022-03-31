#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "renderingEngine/renderingEngine.hpp"

using namespace renderingEngine;

TEST(renderingEngine, true)
{
    {
        RenderingEngine re{};
        re.run();
    }
    EXPECT_TRUE(true);
}