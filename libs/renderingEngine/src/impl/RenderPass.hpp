#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "FrameBufferAttachment.hpp"

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
    FrameBufferAttachment frameBufferAttachments[3];
    FrameBufferAttachment depth;
    VkFramebuffer frameBuffer = VK_NULL_HANDLE;

    VkImageView depthView = VK_NULL_HANDLE; // // we have 2 views of depth attachment, one is created to use in in depth reconstruction
                           // (read only) and one if for normal Z-Buffer write/tests

    VkSampler colorSampler = VK_NULL_HANDLE;
    VkSemaphore deferredSemaphore = VK_NULL_HANDLE;
    VkSemaphore resolvingSemaphore = VK_NULL_HANDLE;
    // std::vector<CShaderProgram*> shaders;
    // CShaderProgram *resolveShader = nullptr;

    std::vector<VkDescriptorImageInfo> descriptionImageInfo;

private:
    void createForwardRenderPass(VkFormat);
    void createDeferredRenderPass(VkFormat);
    void createDeferredFramebuffer();

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
