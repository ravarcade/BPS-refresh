#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct QueueFamilyIndices
{
	int graphicsFamily = -1;
	int presentFamily = -1;
	int transferFamily = -1;

	bool isComplete()
	{
		return graphicsFamily >= 0 and presentFamily >= 0 and transferFamily >= 0;
	}

	QueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface);
};
} // namespace renderingEngine
