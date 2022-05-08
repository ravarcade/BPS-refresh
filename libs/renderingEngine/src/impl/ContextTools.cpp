#include <math.h>
#include "CommandPools.hpp"
#include "Context.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "Texture.hpp"
#include "common/Image.hpp"

namespace
{
using namespace common;
uint32_t mipLevels(uint32_t x, uint32_t y)
{
    return static_cast<uint32_t>(floor(log2(std::max(x, y))) + 1);
}
} // namespace

namespace renderingEngine
{
void Context::createImage(Texture& out, Image& img)
{
    out.format = VK_FORMAT_R8G8B8A8_UNORM;
    if (img.format != PixelFormat::R8G8B8_UNORM)
    {
        throw std::runtime_error("pixel format not supported");
    }

    out.mipLevels = mipLevels(img.width, img.height);
    bool isBuildMipmapsRequired = img.num_mipmaps != out.mipLevels;

    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(phyDev->physicalDevice, out.format, &formatProperties);
    bool isBlitSupported = (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_SRC_BIT) &&
        (formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_BLIT_DST_BIT);

    if (!isBlitSupported && isBuildMipmapsRequired)
    { // fallback to "no mipmaps"...
        isBuildMipmapsRequired = false;
        out.mipLevels = img.num_mipmaps;
    }

    createImage(
        out.image,
        out.memory,
        img.width,
        img.height,
        out.mipLevels,
        img.num_faces * img.num_layers,
        VK_SAMPLE_COUNT_1_BIT,
        out.format,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
            (isBuildMipmapsRequired ? VK_IMAGE_USAGE_TRANSFER_SRC_BIT : 0),
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        (img.isCube ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0) | (img.isArray ? 0 : 0));

    out.view = createImageView(
        out.image, out.format, VK_IMAGE_ASPECT_COLOR_BIT, img.isCube ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D);
}

void Context::copyToStagingBuffer(common::Image& img, uint32_t mip)
{
    if (mip == static_cast<uint32_t>(-1))
    {
        copyToStagingBuffer(img.getRawImage());
    }
    else
    {
        copyToStagingBuffer(img.getMipmap(mip, 0, 0));
    }
}

void Context::copyToStagingBuffer(MemoryBuffer memBuf)
{
    createBuffer(
        memBuf.size(),
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory);

    void* pDevMem = nullptr;
    vkMapMemory(device, stagingBufferMemory, 0, memBuf.size(), 0, &pDevMem);
    memcpy(pDevMem, memBuf.data(), memBuf.size());
    vkUnmapMemory(device, stagingBufferMemory);
}

void Context::transitionImageLayout(
    VkImage image,
    uint32_t baseMipmap,
    uint32_t mipmapsCount,
    uint32_t baseLayer,
    uint32_t layersCount,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        commandBuffer = commandPools->beginSingleTimeCommands();
    }

    VkImageMemoryBarrier barrier = {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = dstAccessMask;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = baseMipmap;
    barrier.subresourceRange.levelCount = mipmapsCount;
    barrier.subresourceRange.baseArrayLayer = baseLayer;
    barrier.subresourceRange.layerCount = layersCount;

    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

void Context::copyBufferToImage(VkImage image, common::Image& img)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        commandBuffer = commandPools->beginSingleTimeCommands();
    }

    // region of dstImage to update
    std::vector<VkBufferImageCopy> bufferCopyRegions;

    for (uint32_t mip = 0; mip < img.num_mipmaps; ++mip)
    {
        for (uint32_t layer = 0; layer < img.num_layers; ++layer)
        {
            for (uint32_t face = 0; face < img.num_faces; ++face)
            {
                auto offset = img.getOffset(mip, layer, face);
                VkBufferImageCopy bufferCopyRegion = {};
                bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                bufferCopyRegion.imageSubresource.mipLevel = mip;
                bufferCopyRegion.imageSubresource.baseArrayLayer =
                    face | layer; // we do "OR" because face = 0 or layer = 0
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageExtent.width = std::max(img.width >> mip, (uint32_t)1);
                bufferCopyRegion.imageExtent.height = std::max(img.height >> mip, (uint32_t)1);
                bufferCopyRegion.imageExtent.depth = 1;
                bufferCopyRegion.bufferOffset = offset;
                bufferCopyRegions.push_back(bufferCopyRegion);
            }
        }
    }

    vkCmdCopyBufferToImage(
        commandBuffer,
        stagingBuffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        static_cast<uint32_t>(bufferCopyRegions.size()),
        bufferCopyRegions.data());
}

void Context::executeSingleTimeCommands(VkCommandBuffer& commandBuffer)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(dev->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(dev->graphicsQueue);

    vkFreeCommandBuffers(device, commandPools->commandPool, 1, &commandBuffer);
    commandBuffer = VK_NULL_HANDLE;
}

void Context::blitImage(VkImage image, uint32_t srcMip, uint32_t dstMip, Image& img)
{
    if (commandBuffer == VK_NULL_HANDLE)
    {
        return;
    }

    VkImageBlit imageBlit{}; // note {} clears stucture!
    // Source
    imageBlit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBlit.srcSubresource.mipLevel = srcMip;
    imageBlit.srcSubresource.baseArrayLayer = 0;
    imageBlit.srcSubresource.layerCount = img.num_faces * img.num_layers;
    imageBlit.srcOffsets[1].x = std::max(int32_t(img.width >> srcMip), 1);
    imageBlit.srcOffsets[1].y = std::max(int32_t(img.height >> srcMip), 1);
    imageBlit.srcOffsets[1].z = 1;

    // Destination
    imageBlit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageBlit.dstSubresource.mipLevel = dstMip;
    imageBlit.dstSubresource.baseArrayLayer = 0;
    imageBlit.dstSubresource.layerCount = img.num_faces * img.num_layers;
    imageBlit.dstOffsets[1].x = std::max(int32_t(img.width >> dstMip), 1);
    imageBlit.dstOffsets[1].y = std::max(int32_t(img.height >> dstMip), 1);
    imageBlit.dstOffsets[1].z = 1;

    vkCmdBlitImage(
        commandBuffer,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        image,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1,
        &imageBlit,
        VK_FILTER_LINEAR);
}

VkSampler Context::createSampler(uint32_t mipmaps, bool enableAnisotrophy)
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.mipLodBias = 0.f;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.minLod = 0.f;
    samplerInfo.maxLod = static_cast<float>(mipmaps);
    samplerInfo.maxAnisotropy = 1.0;
    samplerInfo.anisotropyEnable = VK_FALSE;
    if (phyDev->devFeatures.samplerAnisotropy && enableAnisotrophy)
    {
        samplerInfo.maxAnisotropy = phyDev->devProperties.limits.maxSamplerAnisotropy;
        samplerInfo.anisotropyEnable = VK_TRUE;
    }
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    VkSampler sampler;
    vkCreateSampler(device, &samplerInfo, ire.allocator, &sampler);
    return sampler;
}
} // namespace renderingEngine
