#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;

struct PhysicalDevice
{
    PhysicalDevice(OutputWindowContext&);
    VkFormat getDepthFormat();

    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
   	VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
	VkPhysicalDeviceProperties devProperties{};
	VkPhysicalDeviceFeatures devFeatures{};
    bool enablePiplineStatistic;
};
} // namespace renderingEngine