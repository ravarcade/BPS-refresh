#include "DescriptorSetManager.hpp"
#include "Context.hpp"
#include "PhysicalDevice.hpp"
#include "shaders/ShaderReflections.hpp"

namespace
{
void SetLimit(uint32_t default_DescriptorSizes[], uint32_t maxLimit, std::initializer_list<VkDescriptorType> desc)
{
    maxLimit =
        maxLimit > 10 ? maxLimit / 5 : maxLimit / 2; // we use max 20% of available resources... never ask for more.
    for (auto i : desc)
    {
        if (default_DescriptorSizes[i] > maxLimit) default_DescriptorSizes[i] = maxLimit;
    }
};
} // namespace

namespace renderingEngine
{
DescriptorSetManager::DescriptorSetManager(Context& context) : context{context}
{
    // check default values and compare with limits:
    VkPhysicalDeviceProperties& physicalDeviceProperties = context.phyDev->devProperties;

    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetSamplers,
        {VK_DESCRIPTOR_TYPE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetSampledImages,
        {VK_DESCRIPTOR_TYPE_SAMPLER,
         VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
         VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetStorageImages,
        {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetUniformBuffers,
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetUniformBuffersDynamic,
        {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetStorageBuffers,
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetStorageBuffersDynamic,
        {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC});
    SetLimit(
        default_DescriptorSizes,
        physicalDeviceProperties.limits.maxDescriptorSetInputAttachments,
        {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT});
    createNewDescriptorPool();
}

DescriptorSetManager::~DescriptorSetManager()
{
    context.vkDestroy(descriptorPool);
    for (auto& dp : oldDescriptorPools)
        context.vkDestroy(dp);
    oldDescriptorPools.clear();
    availableDescriptorSets = 0;
}

VkDescriptorSet DescriptorSetManager::createDescriptorSets(
    DescriptorSetLayouts& descriptorSetLayouts,
    DescriptorPoolSizes& descriptorRequirments)
{
    // check if we need new pool
    bool needNewPool = descriptorPool == VK_NULL_HANDLE || !availableDescriptorSets;
    for (uint32_t i = 0; i < descriptorRequirments.size(); ++i)
    {
        if (needNewPool ||
            availableDescriptorTypes[descriptorRequirments[i].type].descriptorCount <
                descriptorRequirments[i].descriptorCount)
        {
            createNewDescriptorPool();
            break;
        }
    }

    --availableDescriptorSets;
    ++stats_usedDescriptorSets;
    for (auto& req : descriptorRequirments)
    {
        availableDescriptorTypes[req.type].descriptorCount -= req.descriptorCount;
        stats_usedDescriptorTypes[req.type] += req.descriptorCount;
    }

    VkDescriptorSet descriptorSet = nullptr;
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    allocInfo.pSetLayouts = descriptorSetLayouts.data();

    if (vkAllocateDescriptorSets(context.device, &allocInfo, &descriptorSet) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to allocate descriptor set!");
    }

    return descriptorSet;
}

void DescriptorSetManager::createNewDescriptorPool()
{
    if (descriptorPool != VK_NULL_HANDLE)
    {
        oldDescriptorPools.push_back(descriptorPool);
        descriptorPool = VK_NULL_HANDLE;
    }

    availableDescriptorSets = default_AvailableDesciprotrSets;
    for (uint32_t i = 0; i < VK_DESCRIPTOR_TYPE_RANGE_SIZE; ++i)
    {
        availableDescriptorTypes[i].type = static_cast<VkDescriptorType>(VK_DESCRIPTOR_TYPE_BEGIN_RANGE + i);
        availableDescriptorTypes[i].descriptorCount = default_DescriptorSizes[i];
    }

    //		_AddOldLimits(shaders, count);

    VkDescriptorPoolSize neededDescriptors[VK_DESCRIPTOR_TYPE_RANGE_SIZE];
    uint32_t neededDescriptorsCount = 0;
    for (auto& desc : availableDescriptorTypes)
    {
        if (desc.descriptorCount > 0)
        {
            neededDescriptors[neededDescriptorsCount] = desc;
            ++neededDescriptorsCount;
        }
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = neededDescriptorsCount;
    poolInfo.pPoolSizes = neededDescriptors;
    poolInfo.maxSets = availableDescriptorSets;

    if (vkCreateDescriptorPool(context.device, &poolInfo, context.ire.allocator, &descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

DescriptorSetLayouts DescriptorSetManager::createDescriptorSetLayouts(ShaderReflections& shaderReflections)
{
    DescriptorSetLayouts dsl;
    auto layout = shaderReflections.resourceLayout;

    // find last set
    uint32_t lastSet = 0, i = 0;
    for (auto& set : layout.descriptorSets)
    {
        if (set.uniform_buffer_mask || set.sampled_image_mask) lastSet = i;
        ++i;
    }

    // create all needed DescriptorSetsLayouts and add it to dsl
    for (uint32_t setNo = 0; setNo <= lastSet; ++setNo)
    {
        auto& set = layout.descriptorSets[setNo];
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        if (set.uniform_buffer_mask)
        {
            for (unsigned i = 0; i < VULKAN_NUM_BINDINGS; ++i)
            {
                uint32_t bitMask = 1u << i;
                uint32_t descriptorCount = set.descriptorCount[i];
                uint32_t stages = set.stages[i];

                if (set.uniform_buffer_mask & bitMask)
                {
                    bindings.push_back({i, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, descriptorCount, stages, nullptr});
                }
            }
        }

        if (set.sampled_image_mask)
        {
            for (unsigned i = 0; i < VULKAN_NUM_BINDINGS; i++)
            {
                uint32_t bitMask = 1u << i;
                uint32_t descriptorCount = set.descriptorCount[i];
                uint32_t stages = set.stages[i];

                if (set.sampled_image_mask & bitMask)
                {
                    bindings.push_back(
                        {i, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, descriptorCount, stages, nullptr});
                }
            }
        }

        if (bindings.size())
        {
            VkDescriptorSetLayoutCreateInfo layoutInfo{
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
                nullptr,
                0,
                static_cast<uint32_t>(bindings.size()),
                bindings.data()};

            VkDescriptorSetLayout descriptorSetLayout;

            if (vkCreateDescriptorSetLayout(context.device, &layoutInfo, context.ire.allocator, &descriptorSetLayout) !=
                VK_SUCCESS)
                throw std::runtime_error("failed to create descriptor set layout!");

            dsl.push_back(descriptorSetLayout);
        }
    }
    return dsl;
}

DescriptorPoolSizes DescriptorSetManager::createDescriptorRequirments(ShaderReflections& /*shaderReflections*/)
{
    DescriptorPoolSizes dps;
    return dps;
}

} // namespace renderingEngine
