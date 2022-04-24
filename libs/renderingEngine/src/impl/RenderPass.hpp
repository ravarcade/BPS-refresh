#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct OutputWindowContext;
// struct CShaderProgram;

enum class RenderPassType
{
    Forward,
    Deferred
};

class RenderPass
{
public:
    RenderPass(RenderPassType, OutputWindowContext&);
    ~RenderPass();

    OutputWindowContext& context;
    VkRenderPass renderPass;

    uint32_t width, height;
    struct FrameBufferAttachment
    {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
        VkFormat format;
        VkImageUsageFlags usage;
    };
    FrameBufferAttachment frameBufferAttachments[3];
    FrameBufferAttachment depth;
    VkFramebuffer frameBuffer;

    VkImageView depthView; // // we have 2 views of depth attachment, one is created to use in in depth reconstruction
                           // (read only) and one if for normal Z-Buffer write/tests

    VkSampler colorSampler;
    VkSemaphore deferredSemaphore;
    VkSemaphore resolvingSemaphore;
    // std::vector<CShaderProgram*> shaders;
    // CShaderProgram *resolveShader = nullptr;

    std::vector<VkDescriptorImageInfo> descriptionImageInfo;

private:
    void createForwardRenderPass(VkFormat);
    void createDeferredRenderPass(VkFormat);
    void createDeferredFramebuffer();
    void createAttachment(VkExtent2D, FrameBufferAttachment&);

    template <typename T>
    void forEachFrameBuffer(T f)
    {
        for (auto& fba : frameBufferAttachments)
        {
            f(fba);
        }
        f(depth);
    }
};
} // namespace renderingEngine
