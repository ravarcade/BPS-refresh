#include "glfwImpl.hpp"
#include <GLFW/glfw3.h>

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

GLFWwindow* GlfwImpl::createWindow(const Rect2D& rect)
{
    auto [width, height] = rect.extent;
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

Rect2D GlfwImpl::getWndSize(GLFWwindow* window)
{
    int width = 0, height = 0;
    glfwGetWindowSize(window, &width, &height);
    return Rect2D{{static_cast<uint32_t>(width), static_cast<uint32_t>(height)}};
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

std::vector<const char*> GlfwImpl::getRequiredVkExtensions()
{
    std::vector<const char*> extensions;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (unsigned int i = 0; i < glfwExtensionCount; i++)
    {
        extensions.push_back(glfwExtensions[i]);
    }

    return extensions;
}
} // namespace renderingEngine