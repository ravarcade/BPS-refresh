#include "renderingEngineImpl.hpp"
#include <ranges>
#include "OutputWindow.hpp"
#include "common/Logger.hpp"
#include "glfwImpl.hpp"
#include "tools/to_vector.hpp"

namespace
{
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char*> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

VkResult createDebugReportCallbackEXT(
    VkInstance instance,
    const VkDebugReportCallbackCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugReportCallbackEXT* pCallback)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
    return func ? func(instance, pCreateInfo, pAllocator, pCallback) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

VkResult createDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    return func ? func(instance, pCreateInfo, pAllocator, pDebugMessenger) : VK_ERROR_EXTENSION_NOT_PRESENT;
}

void destroyDebugReportCallbackEXT(
    VkInstance instance,
    VkDebugReportCallbackEXT& callback,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
        callback = VK_NULL_HANDLE;
    }
}

void destroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT& callback,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance, callback, pAllocator);
        callback = VK_NULL_HANDLE;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugReportFlagsEXT /*flags*/,
    VkDebugReportObjectTypeEXT /*objType*/,
    uint64_t /*obj*/,
    size_t /*location*/,
    int32_t /*code*/,
    const char* /*layerPrefix*/,
    const char* msg,
    void* /*userData*/)
{
    log_err("VK validation layer error: {}", msg);
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL secondDebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* /*pUserData*/) 
{
	if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
    	log_err("VK validation layer error: {}", pCallbackData->pMessage);
	}
	return VK_FALSE;
}
} // namespace

namespace renderingEngine
{
RenderingEngineImpl::RenderingEngineImpl(GlfwImpl& glfw) : glfw{glfw}
{
    init();
}

RenderingEngineImpl::~RenderingEngineImpl()
{
    cleanup();
}

void RenderingEngineImpl::createWindow(const Rect2D& rect)
{
    windows.emplace_back(std::make_unique<OutputWindow>(rect, *this, glfw));
}

const std::vector<const char*>& RenderingEngineImpl::getRequiredDeviceExtensions()
{
    return deviceExtensions;
}

const std::vector<const char*>& RenderingEngineImpl::getValidationLayers()
{
    return validationLayers;
}

bool RenderingEngineImpl::enableValidationLayers()
{
    return not validationLayers.empty();
}

bool RenderingEngineImpl::enablePiplineStatistic()
{
    return true;
}

void RenderingEngineImpl::init()
{
    createInstance();
    setupDebugCallback();
}

void RenderingEngineImpl::cleanup()
{
    windows.clear();
    destroyDebugReportCallbackEXT(instance, debugCallbackBackup, allocator);
    destroyDebugUtilsMessengerEXT(instance, debugMessengerCallbackBackup, allocator);
    vkDestroyInstance(instance, allocator);
}

void RenderingEngineImpl::createInstance()
{
    listVKExtensions();
    if (enableValidationLayers() and !isValidationLayerSupported())
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

    if (vkCreateInstance(&createInfo, allocator, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create instance!");
    }

	log_inf("Vk instance created");
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

    for (const std::string_view layerName : validationLayers)
    {
        if (std::ranges::none_of(
                availableLayers, [layerName](const auto& avalilable) { return layerName == avalilable.layerName; }))
        {
            return false;
        }
    }

    return true;
}

std::vector<const char*> RenderingEngineImpl::getRequiredExtensions()
{
    std::vector<const char*> extensions = glfw.getRequiredVkExtensions();
    if (enableValidationLayers())
    {
        extensions.insert(extensions.end(), {VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME});
    }

    return extensions;
}

std::vector<const char*> RenderingEngineImpl::getRequiredLayers()
{
    return enableValidationLayers() ? validationLayers : std::vector<const char*>{};
}

void RenderingEngineImpl::setupDebugCallback()
{
    if (enableValidationLayers())
    {
        {
            VkDebugReportCallbackCreateInfoEXT createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
            createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
            createInfo.pfnCallback = debugCallback;
            createInfo.pUserData = this;

            if (createDebugReportCallbackEXT(instance, &createInfo, allocator, &debugCallbackBackup) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to set up debug callback!");
            }
        }
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            createInfo.pfnUserCallback = secondDebugCallback;
            createInfo.pUserData = this;

            if (createDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessengerCallbackBackup) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }

        log_inf("Vk debug layer is active");
    }
}
} // namespace renderingEngine