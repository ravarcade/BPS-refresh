#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
class GlfwImpl;
class OutputWindowImpl;

class RenderingEngineImpl
{
public:
    RenderingEngineImpl(GlfwImpl&);
    ~RenderingEngineImpl();

    void createWindow(int width, int height);

    static const std::vector<const char*> validationLayers;
    static const std::vector<const char*> deviceExtensions;
 
private:
    void init();
    void cleanup();
    void createInstance();
    void setupDebugCallback();
    void listVKExtensions();
    bool isValidationLayerSupported();
    std::vector<const char *>getRequiredExtensions();
    std::vector<const char *>getRequiredLayers();

    bool enableValidationLayers{true};
    GlfwImpl& glfw;
    std::vector<std::unique_ptr<OutputWindowImpl>> windows;

	VkInstance instance = VK_NULL_HANDLE;
    VkDebugReportCallbackEXT debugCallbackBackup = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerCallbackBackup = VK_NULL_HANDLE;
	const VkAllocationCallbacks* allocator = VK_NULL_HANDLE;
};
} // namespace renderingEngine