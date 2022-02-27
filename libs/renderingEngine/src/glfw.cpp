#include "renderingEngine/glfw.hpp"
#include <chrono>
#include <glfw\glfw3.h>
#include <thread>

namespace
{
using namespace std::chrono_literals;
bool run = true;
void window_close_callback(GLFWwindow* /*wnd*/)
{
    run = false;
    // reinterpret_cast<iInputCallback*>(glfwGetWindowUserPointer(wnd))->_iglfw_close();
}

} // namespace

namespace renderingEngine
{
int Glfw::GLFW_LastErr = 0;
const char* Glfw::GLFW_LastErrStr = nullptr;

Glfw::Glfw()
{
    glfwSetErrorCallback(errorCallback);
    glfwInit();
}

Glfw::~Glfw()
{
    windows.clear();
    glfwTerminate();
}

GLFWwindow* Glfw::createWindow(int width, int height)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    auto window = glfwCreateWindow(width, height, "Rendering Engine: Vulkan", nullptr, nullptr);
    glfwSetWindowCloseCallback(window, window_close_callback);

    windows.emplace_back(window);

    return window;
}

void Glfw::update()
{
    if (windows.size())
    {
        glfwPollEvents();
        std::erase_if(windows, [](auto& wnd) { return glfwWindowShouldClose(wnd.get()); });
    }
}

void Glfw::runTillStop()
{
    while (run)
    {
        update();
        std::this_thread::sleep_for(10ms);
    }
}

void Glfw::windowDeleter::operator()(GLFWwindow* wnd)
{
    if (wnd)
    {
        glfwDestroyWindow(wnd);
    }
}

void Glfw::errorCallback(int err, const char* err_str)
{
    GLFW_LastErr = err;
    GLFW_LastErrStr = err_str;
}
} // namespace renderingEngine