#include "CommandPools.hpp"
#include <stdexcept>
#include "IRenderingEngine.hpp"
#include "LogicalDevice.hpp"
#include "OutputWindowContext.hpp"
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "Surface.hpp"

namespace renderingEngine
{
CommandPools::CommandPools(OutputWindowContext& context) : context{context}
{
    auto& physicalDevice = context.phyDev->physicalDevice;
    auto& allocator = context.ire.allocator;
    auto& surface = context.surface->surface;
    auto& device = context.device;
    QueueFamilyIndices queueFamilyIndices(physicalDevice, surface);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = 0; // Optional

    if (vkCreateCommandPool(device, &poolInfo, allocator, &commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create command pool!");
    }

    if (queueFamilyIndices.graphicsFamily != queueFamilyIndices.transferFamily)
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamilyIndices.transferFamily;
        poolInfo.flags = 0; // Optional

        if (vkCreateCommandPool(device, &poolInfo, allocator, &transferPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }
}

CommandPools::~CommandPools()
{
    context.vkDestroy(transferPool);
    context.vkDestroy(commandPool);
}

VkCommandBuffer CommandPools::beginSingleTimeCommands()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(context.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void CommandPools::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(context.dev->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(context.dev->graphicsQueue);

    vkFreeCommandBuffers(context.device, commandPool, 1, &commandBuffer);
}
} // namespace renderingEngine
