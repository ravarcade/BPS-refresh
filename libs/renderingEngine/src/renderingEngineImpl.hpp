#pragma once
#include <memory>
#include <vector>
#include <string_view>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
class Glfw;

class RenderingEngineImpl
{
public:
    RenderingEngineImpl(Glfw&);
    ~RenderingEngineImpl();

    void init();

private:
    void createInstance();
    void listVKExtensions();
    bool isValidationLayerSupported();
    std::vector<const char *>getRequiredExtensions();
    std::vector<const char *>getRequiredLayers();

    bool enableValidationLayers{true};
    Glfw& glfw;

    // Vk(Instance);
	VkInstance _instance = VK_NULL_HANDLE;
	const VkAllocationCallbacks* _allocator = VK_NULL_HANDLE;
};
} // namespace renderingEngine