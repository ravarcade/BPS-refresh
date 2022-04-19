#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct IRenderingEngine;
struct PhysicalDevice;

struct CommandPools
{
    CommandPools(IRenderingEngine&, PhysicalDevice&, VkSurfaceKHR&);
    ~CommandPools();

    IRenderingEngine& ire;
    VkCommandPool commandPool;
	VkCommandPool transferPool;
};
} // namespace renderingEngine
