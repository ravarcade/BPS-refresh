#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "common/Rect2D.hpp"

class GLFWwindow;

namespace renderingEngine
{
class IRenderingEngine;
class GlfwImpl;
class PhysicalDevice;
class LogicalDevice;

class OutputWindowImpl
{
public:
    OutputWindowImpl(
        const Rect2D&,
        IRenderingEngine&,
        GlfwImpl&);

    ~OutputWindowImpl();

private:
    void prepare();
    void selectPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice);

    IRenderingEngine& ire;
    GlfwImpl& glfw;
    GLFWwindow* window;

    VkSurfaceKHR surface;
    // VkPhysicalDevice physicalDevice;
    // VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

    std::unique_ptr<PhysicalDevice> phyDev;
    std::unique_ptr<LogicalDevice> dev;

    VkDevice device;
    // VkPhysicalDeviceProperties devProperties;
    // VkPhysicalDeviceFeatures devFeatures;
};
} // namespace renderingEngine