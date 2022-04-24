#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;

struct LogicalDevice
{
    LogicalDevice(OutputWindowContext&);
    ~LogicalDevice();

    OutputWindowContext& context;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
};
} // namespace renderingEngine
