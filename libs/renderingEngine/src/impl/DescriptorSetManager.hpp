#pragma once
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;
struct ShaderReflections;
using DescriptorSetLayouts = std::vector<VkDescriptorSetLayout>;
using DescriptorPoolSizes = std::vector<VkDescriptorPoolSize>;

struct DescriptorSetManager
{
    DescriptorSetManager(Context&);
    ~DescriptorSetManager();

    Context& context;
    VkDescriptorSet createDescriptorSets(DescriptorSetLayouts&, DescriptorPoolSizes&);
    DescriptorSetLayouts createDescriptorSetLayouts(ShaderReflections&);
    DescriptorPoolSizes createDescriptorRequirments(ShaderReflections&);

private:
    void createNewDescriptorPool();
    static constexpr size_t VK_DESCRIPTOR_TYPE_RANGE_SIZE =
        (VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT - VK_DESCRIPTOR_TYPE_SAMPLER + 1);
    static constexpr size_t VK_DESCRIPTOR_TYPE_BEGIN_RANGE = VK_DESCRIPTOR_TYPE_SAMPLER;

    VkDescriptorPoolSize availableDescriptorTypes[VK_DESCRIPTOR_TYPE_RANGE_SIZE];
    uint32_t availableDescriptorSets;

    VkDescriptorPool descriptorPool = VK_NULL_HANDLE;
    std::vector<VkDescriptorPool> oldDescriptorPools;

    // void _CreateNewDescriptorPool();
    // void _AddOldLimits(CShaderProgram **shaders, uint32_t count);

    // params:
    uint32_t default_AvailableDesciprotrSets = 100;
    uint32_t default_DescriptorSizes[VK_DESCRIPTOR_TYPE_RANGE_SIZE] = {
        0, // VK_DESCRIPTOR_TYPE_SAMPLER = 0,
        20, // VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
        0, // VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
        0, // VK_DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
        0, // VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
        0, // VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
        10, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
        0, // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
        0, // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
        0, // VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
    };

    // stats:
    uint32_t stats_usedDescriptorSets = 0;
    uint32_t stats_usedDescriptorTypes[VK_DESCRIPTOR_TYPE_RANGE_SIZE] = {0};
};
} // namespace renderingEngine
