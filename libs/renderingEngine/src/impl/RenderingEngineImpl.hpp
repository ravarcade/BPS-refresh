#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "IRenderingEngine.hpp"

namespace renderingEngine
{
class GlfwImpl;
class OutputWindow;

class RenderingEngineImpl : public IRenderingEngine
{
public:
    RenderingEngineImpl(GlfwImpl&);
    ~RenderingEngineImpl();

    void createWindow(const Rect2D&) override;
    const std::vector<const char*>& getRequiredDeviceExtensions() override;
    const std::vector<const char*>& getValidationLayers() override;
    bool enableValidationLayers() override;
    bool enablePiplineStatistic() override;

private:
    void init();
    void cleanup();
    void createInstance();
    void setupDebugCallback();
    void listVKExtensions();
    bool isValidationLayerSupported();
    std::vector<const char *>getRequiredExtensions();
    std::vector<const char *>getRequiredLayers();

    GlfwImpl& glfw;
    std::vector<std::unique_ptr<OutputWindow>> windows;

    VkDebugReportCallbackEXT debugCallbackBackup = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT debugMessengerCallbackBackup = VK_NULL_HANDLE;
};
} // namespace renderingEngine