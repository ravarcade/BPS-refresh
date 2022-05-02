#pragma once
#include <memory>
#include "common/Rect2D.hpp"
#include <functional>

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
    void runTillStop(std::function<void()>);
    GlfwImpl& getImpl();

private:
    std::unique_ptr<GlfwImpl> impl;
};
} // namespace renderingEngine