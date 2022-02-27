#pragma once
#include <memory>
#include <vector>

struct GLFWwindow;

namespace renderingEngine
{
class Glfw
{
public:
    Glfw();
    ~Glfw();

    GLFWwindow* createWindow(int width, int height);
    void update();
    void runTillStop();

private:
    struct windowDeleter
    {
        void operator()(GLFWwindow*);
    };
    std::vector<std::unique_ptr<GLFWwindow, windowDeleter>> windows;

    static void errorCallback(int err, const char* err_str);
    static int GLFW_LastErr;
    static const char* GLFW_LastErrStr;
};
} // namespace renderingEngine