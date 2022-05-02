#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;
struct FrameBufferAttachment
{
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
    VkImageUsageFlags usage;
};
} // namespace renderingEngine
