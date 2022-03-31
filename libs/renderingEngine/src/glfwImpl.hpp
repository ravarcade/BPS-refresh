#pragma once
#include <memory>
#include <vector>

struct GLFWwindow;

namespace renderingEngine
{
class GlfwImpl
{
public:
    GlfwImpl();
    ~GlfwImpl();

    GLFWwindow* createWindow(int width, int height);
    bool onUpdate();

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