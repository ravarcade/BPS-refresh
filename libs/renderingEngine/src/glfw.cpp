#include "renderingEngine/glfw.hpp"
#include "glfwImpl.hpp"
#include <chrono>
#include <glfw\glfw3.h>
#include <thread>

namespace
{
using namespace std::chrono_literals;
} // namespace

namespace renderingEngine
{
Glfw::Glfw()
    : impl{std::make_unique<GlfwImpl>()}
{
}

Glfw::~Glfw() = default;

GLFWwindow* Glfw::createWindow(int width, int height)
{
    return impl->createWindow(width, height);
}

void Glfw::runTillStop()
{
    while (impl->onUpdate())
    {
        std::this_thread::sleep_for(10ms);
    }
}

} // namespace renderingEngine