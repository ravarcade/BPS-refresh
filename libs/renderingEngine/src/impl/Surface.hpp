#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct GLFWwindow;
namespace renderingEngine
{
struct Context;
struct Surface
{
    Surface(Context&);
    ~Surface();

    VkExtent2D getVkExtentSize();
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat();

    Context& context;
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
};
} // namespace renderingEngine
