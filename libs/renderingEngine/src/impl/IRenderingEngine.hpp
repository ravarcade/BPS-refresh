#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include "common/Rect2D.hpp"

namespace renderingEngine
{
struct IRenderingEngine
{
    virtual void createWindow(const Rect2D& rect) = 0;
    virtual const std::vector<const char*>& getRequiredDeviceExtensions() = 0;
    virtual const std::vector<const char*>& getValidationLayers() = 0;
    virtual bool enableValidationLayers() = 0;
    virtual bool enablePiplineStatistic() = 0;

    VkInstance instance = VK_NULL_HANDLE;
    const VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
};
} // namespace renderingEngine
