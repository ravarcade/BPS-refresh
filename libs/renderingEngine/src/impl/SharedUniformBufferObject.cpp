#include "SharedUniformBufferObject.hpp"
#include "Context.hpp"

namespace renderingEngine
{
SharedUniformBufferObject::SharedUniformBufferObject(Context& context) : context(context)
{
    VkDeviceSize bufferSize = sizeof(SharedUbo);
    VkMemoryPropertyFlags memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    context.createBuffer(
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        memoryProperties,
        sharedUniformBuffer,
        sharedUniformBufferMemory);

    vkMapMemory(context.device, sharedUniformBufferMemory, 0, bufferSize, 0, (void**)&sharedUboData);
}

SharedUniformBufferObject::~SharedUniformBufferObject()
{
    if (sharedUboData)
    {
        vkUnmapMemory(context.device, sharedUniformBufferMemory);
        sharedUboData = nullptr;
    }

    context.vkDestroy(sharedUniformBuffer);
    context.vkFree(sharedUniformBufferMemory);
}
} // namespace renderingEngine
