#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "renderingEngine/renderingEngine.hpp"

using namespace renderingEngine;

TEST(renderingEngine, true)
{
    {
        Glfw glfw;
        glfw.createWindow(300, 300);
        glfw.createWindow(300, 300);
        glfw.createWindow(300, 300);
        glfw.createWindow(300, 300);
        glfw.runTillStop();
    }
    EXPECT_TRUE(true);
}