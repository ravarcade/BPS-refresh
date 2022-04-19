#include "CommandPools.hpp"
#include <stdexcept>
#include "IRenderingEngine.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"

namespace renderingEngine
{
CommandPools::CommandPools(IRenderingEngine& ire, PhysicalDevice& phyDev, VkSurfaceKHR& surface) : ire{ire}
{
    QueueFamilyIndices queueFamilyIndices(phyDev.physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(ire.device, &poolInfo, ire.allocator, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.transferFamily)
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily;
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(ire.device, &poolInfo, ire.allocator, &transferPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
}

CommandPools::~CommandPools()
{
    ire.vkDestroy(transferPool);
    ire.vkDestroy(commandPool);
}
} // namespace renderingEngine
