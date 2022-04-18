#include "renderingEngine/glfw.hpp"
#include "impl\glfwImpl.hpp"
#include <chrono>
#include <GLFW/glfw3.h>
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

GlfwImpl &Glfw::getImpl()
{
    return *impl; 
}

} // namespace renderingEngine