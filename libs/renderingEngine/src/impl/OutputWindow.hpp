#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "Context.hpp"

struct GLFWwindow;
namespace renderingEngine
{
class OutputWindow : public Context
{
public:
    OutputWindow(const Rect2D&, IRenderingEngine&, GlfwImpl&);

    ~OutputWindow();

private:
    void prepare();


};
} // namespace renderingEngine