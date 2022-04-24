#include "Semaphores.hpp"
#include <stdexcept>
#include "IRenderingEngine.hpp"
#include "OutputWindowContext.hpp"

namespace renderingEngine
{
Semaphores::Semaphores(OutputWindowContext& context) : context{context}
{
    auto& device = context.device;
    auto& allocator = context.ire.allocator;
    VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (vkCreateSemaphore(device, &semaphoreInfo, allocator, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, allocator, &renderFinishedSemaphore) != VK_SUCCESS)
    {
		throw std::runtime_error("failed to create semaphores!");
	}
}

Semaphores::~Semaphores()
{
    context.vkDestroy(renderFinishedSemaphore);
    context.vkDestroy(imageAvailableSemaphore);
}
} // namespace renderingEngine
