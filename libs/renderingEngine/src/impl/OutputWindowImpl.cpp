#include "outputWindowImpl.hpp"
#include <GLFW/glfw3.h>
#include <set>
#include "CommandPools.hpp"
#include "LogicalDevice.hpp"
#include "PhysicalDevice.hpp"
#include "Semaphores.hpp"
#include "QueueFamilyIndices.hpp"
#include "SwapChainSupportDetails.hpp"
#include "glfwImpl.hpp"
#include "renderingEngineImpl.hpp"

namespace renderingEngine
{
OutputWindowImpl::OutputWindowImpl(const Rect2D& rect, IRenderingEngine& ire, GlfwImpl& glfw)
    : ire{ire}
	, glfw{glfw}
    , window{glfw.createWindow(rect)}
{
    prepare();
}

OutputWindowImpl::~OutputWindowImpl()
{
	ire.vkDestroy(surface);
}

void OutputWindowImpl::prepare()
{
	if (glfwCreateWindowSurface(ire.instance, window, ire.allocator, &surface) != VK_SUCCESS)
		throw std::runtime_error("failed to create window surface!");
    phyDev = std::make_unique<PhysicalDevice>(ire, surface);
	dev = std::make_unique<LogicalDevice>(ire, *phyDev, surface);
	cmd = std::make_unique<CommandPools>(ire, *phyDev, surface);
	syn = std::make_unique<Semaphores>(ire, *phyDev, surface);
}
} // namespace renderingEngine