#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "FrameBufferAttachment.hpp"

namespace renderingEngine
{
struct Context;

class RenderPass
{
protected:
    RenderPass(Context& context) : context{context} {};

public:
    virtual ~RenderPass(){};

    virtual void createCommandBuffer(VkCommandBuffer&) = 0;

    Context& context;
    VkRenderPass renderPass;

    uint32_t width, height;
    FrameBufferAttachment frameBufferAttachments[3];
    FrameBufferAttachment& colorFba = frameBufferAttachments[0];
    FrameBufferAttachment depthFba;
    VkFramebuffer frameBuffer = VK_NULL_HANDLE;

    VkImageView depthView = VK_NULL_HANDLE; // // we have 2 views of depth attachment, one is created to use in in depth
                                            // reconstruction (read only) and one if for normal Z-Buffer write/tests

    VkSampler colorSampler = VK_NULL_HANDLE;
    VkSemaphore deferredSemaphore = VK_NULL_HANDLE;
    VkSemaphore resolvingSemaphore = VK_NULL_HANDLE;
    // std::vector<CShaderProgram*> shaders;
    // CShaderProgram *resolveShader = nullptr;

    std::vector<VkDescriptorImageInfo> descriptionImageInfo;
};
} // namespace renderingEngine
