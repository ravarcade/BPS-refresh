#include "Semaphores.hpp"
#include <stdexcept>
#include "IRenderingEngine.hpp"

namespace renderingEngine
{
Semaphores::Semaphores(IRenderingEngine& ire, PhysicalDevice& phyDev, VkSurfaceKHR& surface)
: ire{ire}
{
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(ire.device, &semaphoreInfo, ire.allocator, &imageAvailableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(ire.device, &semaphoreInfo, ire.allocator, &renderFinishedSemaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create semaphores!");
	}
}

Semaphores::~Semaphores()
{
    ire.vkDestroy(renderFinishedSemaphore);
    ire.vkDestroy(imageAvailableSemaphore);
}
} // namespace renderingEngine
