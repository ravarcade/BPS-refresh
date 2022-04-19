#pragma once
#include <memory>
#include <vector>
#include <string_view>
#include "common/Rect2D.hpp"

struct GLFWwindow;

namespace renderingEngine
{
class GlfwImpl
{
public:
    GlfwImpl();
    ~GlfwImpl();

    GLFWwindow* createWindow(const Rect2D&);
    bool onUpdate();
    std::vector<const char *>getRequiredVkExtensions();

private:
    void onWindowClose(GLFWwindow*);
    bool isMainWindowOpen{false};

    struct windowDeleter
    {
        void operator()(GLFWwindow* wnd);
    };
    std::vector<std::unique_ptr<GLFWwindow, windowDeleter>> windows;

    static void errorCallback(int err, const char* err_str);
    static void windowCloseCallback(GLFWwindow*);

    static int GLFW_LastErr;
    static const char* GLFW_LastErrStr;
};
} // namespace renderingEngine