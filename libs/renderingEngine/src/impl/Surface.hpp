#pragma once
#include <vector>
#include <vulkan/vulkan.h>

struct GLFWwindow;
namespace renderingEngine
{
struct OutputWindowContext;
struct Surface
{
    Surface(OutputWindowContext&);
    ~Surface();

    VkExtent2D getVkExtentSize();
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR&);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>&);

    OutputWindowContext& context;
    GLFWwindow* window;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
};
} // namespace renderingEngine
