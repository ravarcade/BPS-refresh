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
}
} // namespace renderingEngine