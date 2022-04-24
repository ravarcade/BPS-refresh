#include "FrameBufferAttachment.hpp"
#include "OutputWindowContext.hpp"

namespace renderingEngine
{
void OutputWindowContext::vkDestroy(FrameBufferAttachment& fba)
{
    vkDestroy(fba.image);
    vkDestroy(fba.view);
    vkFree(fba.memory);
}

void OutputWindowContext::createAttachment(VkExtent2D extent, FrameBufferAttachment& attachment)
{
    VkImageAspectFlags aspectFlags = 0;
    VkImageLayout imageLayout;
    uint32_t mipLevels = 1;
    VkSampleCountFlagBits numSamples = msaaSamples;
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkMemoryPropertyFlags properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (attachment.usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        //		usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
        aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
        imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
    if (attachment.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    createImage(
        attachment.image,
        attachment.memory,
        extent.width,
        extent.height,
        mipLevels,
        1,
        numSamples,
        attachment.format,
        tiling,
        attachment.usage,
        properties);
    attachment.view = createImageView(attachment.image, attachment.format, aspectFlags);
    transitionImageLayout(attachment.image, attachment.format, VK_IMAGE_LAYOUT_UNDEFINED, imageLayout);
}

void OutputWindowContext::createAttachment(
    VkFormat format,
    VkImageUsageFlags usage,
    VkExtent2D extent,
    FrameBufferAttachment& attachment)
{
    attachment.format = format;
	attachment.usage = usage;
	createAttachment(extent, attachment);
}
} // namespace renderingEngine