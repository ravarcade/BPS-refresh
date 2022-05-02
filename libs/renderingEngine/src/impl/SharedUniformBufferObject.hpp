#pragma once
#include "glm/glm.hpp"
#include "vulkan/vulkan.h"

namespace renderingEngine
{
struct Context;
struct SharedUniformBufferObject
{
    SharedUniformBufferObject(Context&);
    ~SharedUniformBufferObject();

    Context& context;
    VkBuffer sharedUniformBuffer = VK_NULL_HANDLE;
    VkDeviceMemory sharedUniformBufferMemory = VK_NULL_HANDLE;

    struct SharedUbo
    {
        // glm::mat4 model; // is push constant now
        glm::mat4 view;
        glm::mat4 proj;
    };
    SharedUbo* sharedUboData = nullptr;
};
} // namespace renderingEngine
