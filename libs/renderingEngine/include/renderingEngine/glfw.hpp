#pragma once
#include <memory>

struct GLFWwindow;

namespace renderingEngine
{
class GlfwImpl;

class Glfw
{
public:
    Glfw();
    ~Glfw();

    GLFWwindow* createWindow(int width, int height);
    void runTillStop();
    GlfwImpl& getImpl();

private:
    std::unique_ptr<GlfwImpl> impl;
};
} // namespace renderingEngine