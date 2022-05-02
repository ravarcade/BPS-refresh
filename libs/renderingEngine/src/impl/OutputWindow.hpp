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

    void draw();

private:
    void prepare();
    bool simpleAcquireNextImage(uint32_t&);
    void simpleQueueSubmit(VkSemaphore&, VkSemaphore&, VkCommandBuffer&);
    void simplePresent(VkSemaphore&, uint32_t);
    void recreateSwapChain();
    
};
} // namespace renderingEngine