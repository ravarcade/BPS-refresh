#include "Surface.hpp"
#include <GLFW/glfw3.h>
#include <limits>
#include <stdexcept>
#include "GlfwImpl.hpp"
#include "OutputWindowContext.hpp"
#include "PhysicalDevice.hpp"

namespace renderingEngine
{
Surface::Surface(OutputWindowContext& context) : context{context}
{
    window = context.glfw.createWindow(context.rect);
    if (glfwCreateWindowSurface(context.ire.instance, window, context.ire.allocator, &surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}

Surface::~Surface()
{
    context.vkDestroy(surface);
}

VkExtent2D Surface::getVkExtentSize()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context.phyDev->physicalDevice, context.surface->surface, &capabilities);
    VkExtent2D extent = chooseSwapExtent(capabilities);

    return extent;
}

VkExtent2D Surface::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    else
    {
        Rect2D rect2D = context.glfw.getWndSize(window);
        VkExtent2D actualExtent = {rect2D.extent.x, rect2D.extent.y};

        actualExtent.width = std::max(
            capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(
            capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

        return actualExtent;
    }
}
} // namespace renderingEngine
