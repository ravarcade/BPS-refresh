#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;
struct CommandPools
{
    CommandPools(Context&);
    ~CommandPools();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer);

    Context& context;
    VkCommandPool commandPool;
	VkCommandPool transferPool;
};
} // namespace renderingEngine
