#include "LogicalDevice.hpp"
#include <exception>
#include <set>
#include <vector>
#include "PhysicalDevice.hpp"
#include "QueueFamilyIndices.hpp"
#include "RenderingEngineImpl.hpp"

namespace renderingEngine
{
LogicalDevice::LogicalDevice(
    VkInstance& instance,
    const VkAllocationCallbacks* allocator,
    PhysicalDevice& phyDev,
    VkSurfaceKHR& surface)
{
    QueueFamilyIndices indices(phyDev.physicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily, indices.transferFamily};

    float queuePriority = 1.0f;
    for (int queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    // features
    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    if (phyDev.enablePiplineStatistic)
    {
        deviceFeatures.pipelineStatisticsQuery = VK_TRUE;
        deviceFeatures.occlusionQueryPrecise = VK_TRUE;
    }
    // createInfo structure to create logical device
    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(RenderingEngineImpl::deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = RenderingEngineImpl::deviceExtensions.data();

    // add validation layer for logical device
    // if (ire::enableValidationLayers)
    if (true)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(RenderingEngineImpl::validationLayers.size());
        createInfo.ppEnabledLayerNames = RenderingEngineImpl::validationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(phyDev.physicalDevice, &createInfo, allocator, &device) != VK_SUCCESS)
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);
    vkGetDeviceQueue(device, indices.transferFamily, 0, &transferQueue);
};

LogicalDevice::~LogicalDevice()
{
    vkDestroy(device);
}
} // namespace renderingEngine
