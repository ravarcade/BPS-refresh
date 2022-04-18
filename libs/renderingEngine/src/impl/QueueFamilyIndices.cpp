#include "QueueFamilyIndices.hpp"
#include <vector>

QueueFamilyIndices::QueueFamilyIndices(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    int backup_transferFamily = -1;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 and queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT and presentSupport)
        {
            graphicsFamily = i;
            presentFamily = i;

            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                backup_transferFamily = i;
            }

            if (isComplete())
            {
                break;
            }
        }

        // allow to select 2 different queues, one for graphics, one for present (only if we don't have one with both)
        if (queueFamily.queueCount > 0 and queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT and graphicsFamily == -1)
        {
            graphicsFamily = i;
            if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT and backup_transferFamily == -1)
            {
                backup_transferFamily = i;
            }
        }

        if (queueFamily.queueCount > 0 and presentSupport and presentFamily == -1)
        {
            presentFamily = i;
        }

        if (queueFamily.queueCount > 0 and queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT and
            (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) // we want different transfer queue than graphics
        {
            transferFamily = i;
        }

        i++;
    }

    if (transferFamily == -1) // in case if we don't have separate transfer queue, use graphics queue
    {
        transferFamily = backup_transferFamily;
    }
}
