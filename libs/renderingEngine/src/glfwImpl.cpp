#include "glfwImpl.hpp"
#include <glfw\glfw3.h>

namespace renderingEngine
{
GlfwImpl::GlfwImpl()
{
    glfwSetErrorCallback(errorCallback);
    glfwInit();
}

GlfwImpl::~GlfwImpl()
{
    windows.clear();
    glfwTerminate();
}

GLFWwindow* GlfwImpl::createWindow(int width, int height)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    auto windowName = windows.empty() ? "BPS-Refresh - Main Window" : "BPS-Refresh - Tools";
    auto window = glfwCreateWindow(width, height, windowName, nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetWindowCloseCallback(window, windowCloseCallback);

    if (windows.empty())
    {
        isMainWindowOpen = true;
    }
    windows.emplace_back(window);

    return window;
}

bool GlfwImpl::onUpdate()
{
    if (windows.size())
    {
        glfwPollEvents();
        std::erase_if(windows, [](auto& wnd) { return glfwWindowShouldClose(wnd.get()); });
    }
    return isMainWindowOpen;
}

void GlfwImpl::onWindowClose(GLFWwindow* wnd)
{
    if (not windows.empty() and windows.at(0).get() == wnd)
    {
        isMainWindowOpen = false;
    }
}

void GlfwImpl::windowDeleter::operator()(GLFWwindow* wnd)
{
    if (wnd)
    {
        glfwDestroyWindow(wnd);
    }
}

int GlfwImpl::GLFW_LastErr = 0;
const char* GlfwImpl::GLFW_LastErrStr = nullptr;

void GlfwImpl::errorCallback(int err, const char* err_str)
{
    GLFW_LastErr = err;
    GLFW_LastErrStr = err_str;
}

void GlfwImpl::windowCloseCallback(GLFWwindow* wnd)
{
    if (auto self = reinterpret_cast<GlfwImpl*>(glfwGetWindowUserPointer(wnd)))
    {
        self->onWindowClose(wnd);
    }
}
} // namespace renderingEngine