#include "PhysicalDevice.hpp"
#include <exception>
#include <set>
#include <string_view>
#include "OutputWindowContext.hpp"
#include "QueueFamilyIndices.hpp"
#include "RenderingEngineImpl.hpp"
#include "Surface.hpp"
#include "SwapChainSupportDetails.hpp"

namespace
{
using namespace renderingEngine;

bool checkDeviceExtensionSupport(IRenderingEngine& ire, VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string_view> requiredExtensions(
        ire.getRequiredDeviceExtensions().begin(), ire.getRequiredDeviceExtensions().end());

    for (const auto& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool isDeviceSuitable(IRenderingEngine& ire, VkPhysicalDevice device, VkSurfaceKHR surface)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    QueueFamilyIndices indices(device, surface);

    bool extensionsSupported = checkDeviceExtensionSupport(ire, device);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() and !swapChainSupport.presentModes.empty();
    }

    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU and
        //		deviceFeatures.geometryShader and
        indices.isComplete() and extensionsSupported and deviceFeatures.samplerAnisotropy and swapChainAdequate;
}
} // namespace

namespace renderingEngine
{
PhysicalDevice::PhysicalDevice(OutputWindowContext& context)
{
    auto& ire = context.ire;
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(ire.instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(ire.instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (isDeviceSuitable(ire, device, context.surface->surface))
        {
            physicalDevice = device;
            // msaaSamples = ire::_GetMaxUsableSampleCount(physicalDevice);
            msaaSamples = VK_SAMPLE_COUNT_1_BIT;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");

    vkGetPhysicalDeviceProperties(physicalDevice, &devProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &devFeatures);

    enablePiplineStatistic = ire.enablePiplineStatistic() and devFeatures.pipelineStatisticsQuery == VK_TRUE and
        devFeatures.occlusionQueryPrecise == VK_TRUE;
}
} // namespace renderingEngine