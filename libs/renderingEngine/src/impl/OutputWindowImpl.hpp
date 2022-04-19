#pragma once
#include <memory>
#include <vulkan/vulkan.h>
#include "common/Rect2D.hpp"

class GLFWwindow;

namespace renderingEngine
{
struct IRenderingEngine;
struct GlfwImpl;
struct PhysicalDevice;
struct LogicalDevice;
struct CommandPools;
struct Semaphores;

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
    std::unique_ptr<CommandPools> cmd;
    std::unique_ptr<Semaphores> syn;

    VkDevice device;
    // VkPhysicalDeviceProperties devProperties;
    // VkPhysicalDeviceFeatures devFeatures;
};
} // namespace renderingEngine