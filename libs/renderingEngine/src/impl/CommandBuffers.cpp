#include "CommandBuffers.hpp"
#include "Context.hpp"
#include "SwapChain.hpp"
#include "CommandPools.hpp"
#include "RenderPass.hpp"

namespace renderingEngine
{
CommandBuffers::CommandBuffers(Context& context) : context{context}
{
	// allocate command buffero (one for every swap-chain-frame-buffer)
	commandBuffers.resize(context.swapChain->framebuffers.size()+1);
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = context.commandPools->commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(context.device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}	


	// viewport
    auto& extent = context.swapChain->extent;
	scissor = { { 0, 0 }, extent};
	viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);


	currentDescriptorSet = nullptr;    
	auto &deferredCommandBuffer = commandBuffers[context.swapChain->framebuffers.size()];
    context.derreferedRenderPass->createCommandBuffer(deferredCommandBuffer);
}

CommandBuffers::~CommandBuffers()
{
    context.vkFree(context.commandPools->commandPool, commandBuffers);
}
} // namespace renderingEngine
