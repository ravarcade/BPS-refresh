#include "renderingEngineImpl.hpp"
#include <ranges>
#include "renderingEngine/glfw.hpp"
#include "glfwImpl.hpp"
#include "common/Logger.hpp"
#include "tools/to_vector.hpp"

namespace {
const std::vector<const char *>validationLayers = { "VK_LAYER_KHRONOS_validation" };
} // namespace

namespace renderingEngine
{
RenderingEngineImpl::RenderingEngineImpl(Glfw& glfw) : glfw{glfw} {}
RenderingEngineImpl::~RenderingEngineImpl() = default;

void RenderingEngineImpl::init()
{
    createInstance();
}

void RenderingEngineImpl::createInstance()
{
    listVKExtensions();
    if (enableValidationLayers && !isValidationLayerSupported())
	{
		throw std::runtime_error("validation layers requested, but not available!");
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "BAMS";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "BAMS Rendering Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensions = getRequiredExtensions();
	auto layers = getRequiredLayers();

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.size() ? extensions.data() : nullptr;
	createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
	createInfo.ppEnabledLayerNames = layers.size() ? layers.data() : nullptr;

	if (vkCreateInstance(&createInfo, _allocator, &_instance) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create instance!");
	}
}

void RenderingEngineImpl::listVKExtensions()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

    auto extensionNames = tools::to_vector(extensions, [](const auto& ext) { return ext.extensionName; });
    log_dbg("Available vulkan extensions: {}", extensionNames);
}

bool RenderingEngineImpl::isValidationLayerSupported()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const auto& layerName : validationLayers)
    {
        if (std::ranges::none_of(
                availableLayers, [layerName](const auto& avalilable) { return layerName == avalilable.layerName; }))
        {
            return false;
        }
    }

    return true;
}

std::vector<const char *>RenderingEngineImpl::getRequiredExtensions()
{
	std::vector<const char *>extensions = glfw.getImpl().getRequiredVkExtensions();

	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

std::vector<const char *>RenderingEngineImpl::getRequiredLayers()
{
	return enableValidationLayers ? validationLayers : std::vector<const char *>{};
}
} // namespace renderingEngine