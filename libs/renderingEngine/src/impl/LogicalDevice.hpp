#pragma once
#include <vulkan/vulkan.h>
#include "IRenderingEngine.hpp"

namespace renderingEngine
{
struct PhysicalDevice;

struct LogicalDevice
{
    LogicalDevice(IRenderingEngine&, PhysicalDevice&, VkSurfaceKHR&);
    ~LogicalDevice();

    IRenderingEngine& ire;
    VkDevice& device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;
};
} // namespace renderingEngine
