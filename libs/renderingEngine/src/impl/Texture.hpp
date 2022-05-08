#pragma once
#include <vulkan/vulkan.h>

namespace common
{
struct Image;
} // namespace common

namespace renderingEngine
{
struct Context;

struct Texture
{
    Texture(Context& context) : context{context} {}
    virtual ~Texture(){};

    Context& context;
    VkDescriptorImageInfo descriptor;

    VkSampler sampler = VK_NULL_HANDLE;
    VkImage image = VK_NULL_HANDLE;
    VkDeviceMemory memory = VK_NULL_HANDLE;
    VkImageView view = VK_NULL_HANDLE;
    VkFormat format;
    VkImageLayout imageLayout;
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t mipLevels;
};
} // namespace renderingEngine
