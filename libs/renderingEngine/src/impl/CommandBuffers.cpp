#include "CommandBuffers.hpp"
#include "CommandPools.hpp"
#include "Context.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"

namespace renderingEngine
{
CommandBuffers::CommandBuffers(Context& context) : context{context}
{
    // allocate command buffero (one for every swap-chain-frame-buffer)
    commandBuffers.resize(context.swapChain->framebuffers.size() + 1);
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = context.commandPools->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

    if (vkAllocateCommandBuffers(context.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    currentDescriptorSet = nullptr;
	VkFramebuffer dummyFrameBuffer = VK_NULL_HANDLE;
    auto& deferredCommandBuffer = commandBuffers[context.swapChain->framebuffers.size()];
    context.derreferedRenderPass->createCommandBuffer(deferredCommandBuffer, dummyFrameBuffer);

    for (size_t i = 0; i < context.swapChain->framebuffers.size(); ++i)
    {
        context.forwardRenderPass->createCommandBuffer(commandBuffers[i], context.swapChain->framebuffers[i]);
    }
}

CommandBuffers::~CommandBuffers()
{
    context.vkFree(context.commandPools->commandPool, commandBuffers);
}
} // namespace renderingEngine
