#pragma once
#include <vulkan/vulkan.h>
#include <memory>

class GLFWwindow;

namespace renderingEngine
{
class GlfwImpl;
class RenderingEngineImpl;
class PhysicalDevice;
class LogicalDevice;

class OutputWindowImpl
{
public:
    OutputWindowImpl(int width, int height, VkInstance& instance, const VkAllocationCallbacks* allocator, GlfwImpl& glfw);
    ~OutputWindowImpl();

private:
    void prepare();
    void selectPhysicalDevice();
    bool isDeviceSuitable(VkPhysicalDevice);
    bool checkDeviceExtensionSupport(VkPhysicalDevice);


    GlfwImpl& glfw;
    GLFWwindow* window;

    VkInstance& vkInstance;
    const VkAllocationCallbacks* vkAllocator;

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