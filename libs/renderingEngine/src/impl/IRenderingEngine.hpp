#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "common/Rect2D.hpp"

namespace renderingEngine
{
struct IRenderingEngine
{
    virtual void createWindow(const Rect2D& rect) = 0;
    virtual const std::vector<const char*>& getRequiredDeviceExtensions() = 0;
    virtual const std::vector<const char*>& getValidationLayers() = 0;
    virtual bool enableValidationLayers() = 0;
    virtual bool enablePiplineStatistic() = 0;

    VkInstance instance = VK_NULL_HANDLE;
    const VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
    VkDevice device;

    template <typename T>
    void vkDestroy(T& v)
    {
        if (v)
        {
            vkDestroyDevice(v, allocator);
            v = VK_NULL_HANDLE;
        }
    }

    void vkDestroy(VkSurfaceKHR& v)
    {
        if (v)
        {
            vkDestroySurfaceKHR(instance, v, allocator);
            v = VK_NULL_HANDLE;
        }
    }

#ifdef Define_vkDestroy
#undef Define_vkDestroy
#endif
#define Define_vkDestroy(t)                     \
    void vkDestroy(Vk##t& v)                    \
    {                                           \
        if (v)                                  \
        {                                       \
            vkDestroy##t(device, v, allocator); \
            v = VK_NULL_HANDLE;                 \
        }                                       \
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
};
} // namespace renderingEngine
