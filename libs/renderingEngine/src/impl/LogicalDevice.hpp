#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;

struct LogicalDevice
{
    LogicalDevice(Context&);
    ~LogicalDevice();

    Context& context;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
};
} // namespace renderingEngine
