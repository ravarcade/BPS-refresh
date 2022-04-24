#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "IRenderingEngine.hpp"
#include "common/Rect2D.hpp"

struct GLFWwindow;

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

struct OutputWindowContext
{
    OutputWindowContext(const Rect2D&, IRenderingEngine&, GlfwImpl&);
    ~OutputWindowContext() = default;

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
    std::unique_ptr<CommandPools> cmd;
    std::unique_ptr<Semaphores> syn;
    std::unique_ptr<RenderPass> rpFwd;
    std::unique_ptr<RenderPass> rpDer;

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