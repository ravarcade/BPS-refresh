#pragma once
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct PhysicalDevice;

struct LogicalDevice
{
    LogicalDevice(VkInstance&, const VkAllocationCallbacks*, PhysicalDevice&, VkSurfaceKHR&);
    ~LogicalDevice();

    template <typename T>
    void vkDestroy(T& v)
    {
        if (v)
        {
            vkDestroyDevice(v, allocator);
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

    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkQueue transferQueue;

    const VkAllocationCallbacks* allocator;
};
} // namespace renderingEngine
