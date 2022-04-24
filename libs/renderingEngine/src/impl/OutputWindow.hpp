#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "OutputWindowContext.hpp"

struct GLFWwindow;

namespace renderingEngine
{
class OutputWindow : public OutputWindowContext
{
public:
    OutputWindow(const Rect2D&, IRenderingEngine&, GlfwImpl&);

    ~OutputWindow();

private:
    void prepare();

    // const Rect2D& rect;
    // IRenderingEngine& ire;
    // GlfwImpl& glfw;

    // std::unique_ptr<Surface> surface;
    // std::unique_ptr<PhysicalDevice> phyDev;
    // std::unique_ptr<LogicalDevice> dev;
    // std::unique_ptr<CommandPools> cmd;
    // std::unique_ptr<Semaphores> syn;
    // std::unique_ptr<RenderPass> rpFwd;
    // std::unique_ptr<RenderPass> rpDer;
};
} // namespace renderingEngine