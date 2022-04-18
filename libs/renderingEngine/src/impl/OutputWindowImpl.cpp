#include "outputWindowImpl.hpp"
#include "renderingEngineImpl.hpp"
#include "glfwImpl.hpp"
#include <GLFW/glfw3.h>
#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include <set>
#include "PhysicalDevice.hpp"
#include "LogicalDevice.hpp"

namespace renderingEngine
{
OutputWindowImpl::OutputWindowImpl(int width, int height, VkInstance& instance, const VkAllocationCallbacks* allocator, GlfwImpl& glfw)
    : glfw{glfw}
    , window{glfw.createWindow(width, height)}
    , vkInstance{instance}
    , vkAllocator{allocator}
{
    prepare();
}

OutputWindowImpl::~OutputWindowImpl() = default;

void OutputWindowImpl::prepare()
{
	if (glfwCreateWindowSurface(vkInstance, window, vkAllocator, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
    phyDev = std::make_unique<PhysicalDevice>(vkInstance, vkAllocator, surface);
	dev = std::make_unique<LogicalDevice>(vkInstance, vkAllocator, *phyDev, surface);
}
} // namespace renderingEngine