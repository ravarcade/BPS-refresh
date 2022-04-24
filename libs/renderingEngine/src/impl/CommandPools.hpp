#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;
struct CommandPools
{
    CommandPools(OutputWindowContext&);
    ~CommandPools();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer);

    OutputWindowContext& context;
    VkCommandPool commandPool;
	VkCommandPool transferPool;
};
} // namespace renderingEngine
