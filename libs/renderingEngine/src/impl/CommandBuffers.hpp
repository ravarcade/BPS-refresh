#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;

struct CommandBuffers
{
    CommandBuffers(Context&);
    ~CommandBuffers();

    Context& context;
    std::vector<VkCommandBuffer> commandBuffers;

    // wrong place
    VkViewport viewport;
    VkRect2D scissor;
    VkDescriptorSet currentDescriptorSet = VK_NULL_HANDLE;
};
} // namespace renderingEngine
