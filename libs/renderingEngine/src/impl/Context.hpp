#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "IRenderingEngine.hpp"
#include "common/MemoryBuffer.hpp"
#include "common/Rect2D.hpp"

struct GLFWwindow;

namespace common
{
struct Image;
} // namespace common

namespace renderingEngine
{
struct CommandPools;
struct GlfwImpl;
struct IRenderingEngine;
struct LogicalDevice;
struct PhysicalDevice;
struct Semaphores;
struct Surface;
struct RenderPass;
struct SwapChain;
struct FrameBufferAttachment;
struct SharedUniformBufferObject;
struct CommandBuffers;
struct PipelineStatistic;
struct DescriptorSetManager;
struct Gui;
struct Texture;

struct Context
{
    Context(const Rect2D&, IRenderingEngine&, GlfwImpl&);
    ~Context() = default;

    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void createImage(
        VkImage& outImage,
        VkDeviceMemory& outImageMemory,
        uint32_t width,
        uint32_t height,
        uint32_t mipLevels,
        uint32_t layers,
        VkSampleCountFlagBits numSamples,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        VkImageCreateFlags flags = 0);
    VkImageView createImageView(
        VkImage image,
        VkFormat format,
        VkImageAspectFlags aspectFlags,
        VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    bool hasStencilComponent(VkFormat);
    void createAttachment(VkExtent2D, FrameBufferAttachment&);
    void createAttachment(VkFormat, VkImageUsageFlags, VkExtent2D, FrameBufferAttachment&);
    void createBuffer(
        VkDeviceSize,
        VkBufferUsageFlags,
        VkMemoryPropertyFlags,
        VkBuffer&,
        VkDeviceMemory&,
        VkSharingMode = VK_SHARING_MODE_EXCLUSIVE);

    // tools
    void createImage(Texture&, common::Image&);
    void copyToStagingBuffer(common::Image&, uint32_t = -1);
    void copyToStagingBuffer(MemoryBuffer);
    void transitionImageLayout(
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
        VkPipelineStageFlags dstStageMask);
    void copyBufferToImage(VkImage, common::Image&);
    void executeSingleTimeCommands(VkCommandBuffer&);
    void blitImage(VkImage image, uint32_t srcMip, uint32_t dstMip, common::Image& img);
    VkSampler createSampler(uint32_t mipmaps, bool enableAnisotrophy);

    const Rect2D& rect;
    GlfwImpl& glfw;
    // VkInstance instance = VK_NULL_HANDLE;
    // const VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
    IRenderingEngine& ire;

    VkDevice device;
    VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;
    std::unique_ptr<Surface> surface;
    std::unique_ptr<PhysicalDevice> phyDev;
    std::unique_ptr<LogicalDevice> dev;
    std::unique_ptr<CommandPools> commandPools;
    std::unique_ptr<Semaphores> syn;
    std::unique_ptr<RenderPass> forwardRenderPass;
    std::unique_ptr<RenderPass> derreferedRenderPass;
    std::unique_ptr<SwapChain> swapChain;
    std::unique_ptr<SharedUniformBufferObject> sharedUBO;
    std::unique_ptr<CommandBuffers> commandBuffers;
    std::unique_ptr<PipelineStatistic> pipelineStatistic;
    std::unique_ptr<DescriptorSetManager> descriptorSetManager;
    std::unique_ptr<Gui> ui;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

    template <typename T>
    void vkDestroy(T& v)
    {
        if (v)
        {
            vkDestroyDevice(v, ire.allocator);
            v = VK_NULL_HANDLE;
        }
    }

    void vkDestroy(VkSurfaceKHR& v)
    {
        if (v)
        {
            vkDestroySurfaceKHR(ire.instance, v, ire.allocator);
            v = VK_NULL_HANDLE;
        }
    }

    void vkDestroy(FrameBufferAttachment&);

#ifdef Define_vkDestroy
#undef Define_vkDestroy
#endif
#define Define_vkDestroy(t)                         \
    void vkDestroy(Vk##t& v)                        \
    {                                               \
        if (v)                                      \
        {                                           \
            vkDestroy##t(device, v, ire.allocator); \
            v = VK_NULL_HANDLE;                     \
        }                                           \
    }

    Define_vkDestroy(Pipeline);
    Define_vkDestroy(PipelineCache);
    Define_vkDestroy(PipelineLayout);
    Define_vkDestroy(ImageView);
    Define_vkDestroy(Image);
    Define_vkDestroy(Framebuffer);
    Define_vkDestroy(RenderPass);
    Define_vkDestroy(SwapchainKHR);
    Define_vkDestroy(DescriptorPool);
    Define_vkDestroy(DescriptorSetLayout);
    Define_vkDestroy(Buffer);
    Define_vkDestroy(Semaphore);
    Define_vkDestroy(CommandPool);
    Define_vkDestroy(ShaderModule);
    Define_vkDestroy(Sampler);
    Define_vkDestroy(QueryPool);

    void vkFree(VkDeviceMemory& v)
    {
        if (v)
        {
            vkFreeMemory(device, v, ire.allocator);
            v = nullptr;
        }
    }

    void vkFree(VkCommandPool& v1, std::vector<VkCommandBuffer>& v2)
    {
        if (v1 && v2.size())
        {
            vkFreeCommandBuffers(device, v1, static_cast<uint32_t>(v2.size()), v2.data());
            v2.clear();
        }
    }

    void vkFree(VkDescriptorPool& v1, std::vector<VkDescriptorSet>& v2)
    {
        if (v1 && v2.size())
        {
            vkFreeDescriptorSets(device, v1, static_cast<uint32_t>(v2.size()), v2.data());
            v2.clear();
        }
    }
};
} // namespace renderingEngine