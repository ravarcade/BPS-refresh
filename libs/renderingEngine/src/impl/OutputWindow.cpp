#include "OutputWindow.hpp"
#include <GLFW/glfw3.h>
#include <set>
#include "CommandBuffers.hpp"
#include "CommandPools.hpp"
#include "DefferedRenderPass.hpp"
#include "DescriptorSetManager.hpp"
#include "ForwardRenderPass.hpp"
#include "Gui.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "PipelineStatistic.hpp"
#include "QueueFamilyIndices.hpp"
#include "Semaphores.hpp"
#include "ShaderProgram.hpp"
#include "SharedUniformBufferObject.hpp"
#include "Surface.hpp"
#include "SwapChain.hpp"
#include "SwapChainSupportDetails.hpp"
#include "glfwImpl.hpp"
#include "renderingEngineImpl.hpp"

namespace renderingEngine
{
OutputWindow::OutputWindow(const Rect2D& rect, IRenderingEngine& ire, GlfwImpl& glfw)
    : Context(rect, ire, glfw)
{
    prepare();
}

OutputWindow::~OutputWindow() {}

void OutputWindow::prepare()
{
    surface = std::make_unique<Surface>(*this);
    phyDev = std::make_unique<PhysicalDevice>(*this);
    dev = std::make_unique<LogicalDevice>(*this);
    commandPools = std::make_unique<CommandPools>(*this);
    syn = std::make_unique<Semaphores>(*this);
    forwardRenderPass = std::make_unique<ForwardRenderPass>(*this);
    derreferedRenderPass = std::make_unique<DefferedRenderPass>(*this);
    swapChain = std::make_unique<SwapChain>(*this);
    sharedUBO = std::make_unique<SharedUniformBufferObject>(*this);
    pipelineStatistic = std::make_unique<PipelineStatistic>(*this);
    descriptorSetManager = std::make_unique<DescriptorSetManager>(*this);
    shaderProgram = std::make_unique<ShaderProgram>(*this);
    commandBuffers = std::make_unique<CommandBuffers>(*this);
    ui = std::make_unique<Gui>(*this);
}

void OutputWindow::draw()
{
    uint32_t imageIndex;
    if (not simpleAcquireNextImage(imageIndex))
		return;

    simpleQueueSubmit(
        syn->imageAvailableSemaphore,
        derreferedRenderPass->deferredSemaphore,
        *commandBuffers->commandBuffers.rbegin());

    pipelineStatistic->update();

    simpleQueueSubmit(
        derreferedRenderPass->deferredSemaphore,
        syn->renderFinishedSemaphore,
        commandBuffers->commandBuffers[imageIndex]);
    simplePresent(syn->renderFinishedSemaphore, imageIndex);
    vkQueueWaitIdle(dev->presentQueue);
}

bool OutputWindow::simpleAcquireNextImage(uint32_t &imageIndex)
{
    VkResult result = vkAcquireNextImageKHR(
        device,
        swapChain->swapChain,
        std::numeric_limits<uint64_t>::max(),
        syn->imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
		recreateSwapChain();
		return false;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	return true;
}

void OutputWindow::simplePresent(VkSemaphore &waitSemaphore, uint32_t imageIndex)
{
	VkSemaphore waitSemaphores[] = { waitSemaphore };

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = waitSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain->swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Optional

	VkResult result = vkQueuePresentKHR(dev->presentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}
}

void OutputWindow::recreateSwapChain()
{
}

void OutputWindow::simpleQueueSubmit(
    VkSemaphore& waitSemaphore,
    VkSemaphore& signalSemaphore,
    VkCommandBuffer& commandBuffer)
{
    VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { waitSemaphore };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { signalSemaphore };

	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	if (vkQueueSubmit(dev->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}
}
} // namespace renderingEngine