#include "renderingEngineImpl.hpp"
#include <ranges>
#include <vulkan/vulkan.h>
#include "common/Logger.hpp"
#include "tools/to_vector.hpp"

namespace renderingEngine
{
RenderingEngineImpl::RenderingEngineImpl() = default;
RenderingEngineImpl::~RenderingEngineImpl() = default;

void RenderingEngineImpl::init()
{
    createInstance();
}

void RenderingEngineImpl::createInstance()
{
    listVKExtensions();
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

} // namespace renderingEngine