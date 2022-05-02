#include "DefferedRenderPass.hpp"
#include "Context.hpp"
#include "PhysicalDevice.hpp"
#include "Surface.hpp"
// #include "CommandBuffers.hpp" - not allowed to use commandBuffers here!
#include "PipelineStatistic.hpp"
#include "SwapChain.hpp"

namespace renderingEngine
{
DefferedRenderPass::DefferedRenderPass(Context& context) : RenderPass(context)
{
    auto msaaSamples = context.msaaSamples;
    auto& device = context.device;
    auto& allocator = context.ire.allocator;
    auto depthBufferFormat = context.phyDev->getDepthFormat();

    VkFormat deferredAttachmentFormats[] = {
        VK_FORMAT_R16G16_SNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32G32B32A32_SFLOAT, depthBufferFormat};
    // VK_FORMAT_R16G16_SNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, depthBufferFormat};
    // VK_FORMAT_A2R10G10B10_UNORM_PACK32, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_R32G32B32A32_SFLOAT, depthBufferFormat};
    // VK_FORMAT_A2B10G10R10_UNORM_PACK32

    VkImageUsageFlags deferredAttachmentUsage[] = {
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT};

    msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    // create render passes
    std::vector<VkAttachmentDescription> attachmentDescs;
    std::vector<VkAttachmentReference> colorReferences;
    VkAttachmentReference depthReference;

    forEachFrameBuffer(
        [&](FrameBufferAttachment& fba)
        {
            uint32_t attachmentIndex = static_cast<uint32_t>(attachmentDescs.size());
            fba.format = deferredAttachmentFormats[attachmentIndex];
            fba.usage = deferredAttachmentUsage[attachmentIndex];

            VkAttachmentDescription desc;
            desc.flags = 0;
            desc.samples = msaaSamples;
            desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            desc.format = fba.format;
            if (fba.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                // desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; // we will read depth
                // buffor, so mark "finalLayout" as read only
                desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                depthReference = {attachmentIndex, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};
            }
            else
            {
                desc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                colorReferences.push_back({attachmentIndex, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
            }
            attachmentDescs.push_back(desc);
        });

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.pColorAttachments = colorReferences.data();
    subpass.colorAttachmentCount = static_cast<uint32_t>(colorReferences.size());
    subpass.pDepthStencilAttachment = &depthReference;
    //	subpass.pResolveAttachments = samples != VK_SAMPLE_COUNT_1_BIT ? &colorAttachmentResolveRef : nullptr;

    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pAttachments = attachmentDescs.data();
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachmentDescs.size());
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 2;
    renderPassInfo.pDependencies = dependencies.data();

    if (vkCreateRenderPass(device, &renderPassInfo, allocator, &renderPass) != VK_SUCCESS)
        throw std::runtime_error("failed to create render pass!");

    // sampler
    VkSamplerCreateInfo sampler{};
    sampler.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    sampler.maxAnisotropy = 1.0f;
    sampler.magFilter = VK_FILTER_NEAREST;
    sampler.minFilter = VK_FILTER_NEAREST;
    sampler.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    sampler.addressModeV = sampler.addressModeU;
    sampler.addressModeW = sampler.addressModeU;
    sampler.mipLodBias = 0.0f;
    sampler.maxAnisotropy = 1.0f;
    sampler.minLod = 0.0f;
    sampler.maxLod = 1.0f;
    sampler.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    if (vkCreateSampler(device, &sampler, nullptr, &colorSampler) != VK_SUCCESS)
        throw std::runtime_error("failed to sampler!");

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device, &semaphoreInfo, allocator, &deferredSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, allocator, &resolvingSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create semaphores!");
    }

    createDeferredFramebuffer();
}

DefferedRenderPass::~DefferedRenderPass()
{
    context.vkDestroy(deferredSemaphore);
    context.vkDestroy(resolvingSemaphore);
    context.vkDestroy(colorSampler);
    context.vkDestroy(frameBuffer);
    context.vkDestroy(renderPass);
    forEachFrameBuffer(
        [&](auto& fba)
        {
            context.vkDestroy(fba);
        });
    context.vkDestroy(depthView);
}

void DefferedRenderPass::createCommandBuffer(VkCommandBuffer& cmdBuf, VkFramebuffer& /*frameBuffer*/)
{
	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	beginInfo.pInheritanceInfo = nullptr; // Optional
    vkBeginCommandBuffer(cmdBuf, &beginInfo);

    auto& viewport = context.swapChain->viewport;
    auto& scissor = context.swapChain->scissor;

	std::array<VkClearValue, 4> clearValues = {};
	clearValues[0].color = { 0.5f, 0.5f, 0.0f, 0.0f };
	clearValues[1].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[2].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearValues[3].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = frameBuffer;

	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = { width, height };
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

    context.pipelineStatistic->reset(cmdBuf);

    vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	vkCmdSetViewport(cmdBuf, 0, 1, &viewport);
	vkCmdSetScissor(cmdBuf, 0, 1, &scissor);

    context.pipelineStatistic->begin(cmdBuf);
	//vkCmdBeginQuery(cb, m_pipelineStatisticsQueryPool, 0, VK_QUERY_CONTROL_PRECISE_BIT);

    // rendering!!!
	// for (auto &shader : shaders) 
	// {
	// 	shader->DrawObjects(cb);
	// }

    context.pipelineStatistic->end(cmdBuf);
    if (context.pipelineStatistic)
	    vkCmdEndQuery(cmdBuf, context.pipelineStatistic->queryPool, 0);

	vkCmdEndRenderPass(cmdBuf);

    if (vkEndCommandBuffer(cmdBuf) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void DefferedRenderPass::createDeferredFramebuffer()
{
    VkExtent2D winExtent = context.surface->getVkExtentSize();
    width = winExtent.width;
    height = winExtent.height;

    std::vector<VkImageView> attachments;
    descriptionImageInfo.clear();

    forEachFrameBuffer(
        [&](FrameBufferAttachment& fba)
        {
            context.createAttachment(winExtent, fba);
            attachments.push_back(fba.view);

            // .... but for depth we need another image view ....
            if (fba.usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            {
                // create different view for "read only"
                depthView = fba.view;
                fba.view = context.createImageView(fba.image, fba.format, VK_IMAGE_ASPECT_DEPTH_BIT);
            }

            descriptionImageInfo.push_back({colorSampler, fba.view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL});
        });

    VkFramebufferCreateInfo fbufCreateInfo = {};
    fbufCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fbufCreateInfo.pNext = NULL;
    fbufCreateInfo.renderPass = renderPass;
    fbufCreateInfo.pAttachments = attachments.data();
    fbufCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    fbufCreateInfo.width = width;
    fbufCreateInfo.height = height;
    fbufCreateInfo.layers = 1;
    if (vkCreateFramebuffer(context.device, &fbufCreateInfo, nullptr, &frameBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to create frame buffer!");
}
} // namespace renderingEngine
