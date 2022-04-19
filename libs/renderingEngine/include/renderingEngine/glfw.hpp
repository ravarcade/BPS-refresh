#pragma once
#include <memory>
#include "common/Rect2D.hpp"

struct GLFWwindow;

namespace renderingEngine
{
class GlfwImpl;

class Glfw
{
public:
    Glfw();
    ~Glfw();

    GLFWwindow* createWindow(const Rect2D&);
    void runTillStop();
    GlfwImpl& getImpl();

private:
    std::unique_ptr<GlfwImpl> impl;
};
} // namespace renderingEngine