#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "ShaderVariable.hpp"
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
constexpr unsigned VULKAN_NUM_DESCRIPTOR_SETS = 4;
constexpr unsigned VULKAN_NUM_BINDINGS = 16;

struct Context;
struct ShaderProgramInfo
{
    // BAMS::CResRawData resource;
    std::string entryPointName;
    VkShaderStageFlagBits stage;
};

struct ResourceLayout
{
    struct DescriptorSetLayout
    {
        uint32_t uniform_buffer_mask = 0;
        uint32_t sampled_image_mask = 0;
        uint32_t descriptorCount[VULKAN_NUM_BINDINGS] = {0};
        uint32_t stages[VULKAN_NUM_BINDINGS] = {0};
    };

    DescriptorSetLayout descriptorSets[VULKAN_NUM_DESCRIPTOR_SETS];
    std::vector<VkPushConstantRange> pushConstants;
};

struct ShaderReflections
{
    ShaderReflections(MemoryBuffer);
    ~ShaderReflections();

    // void reset(VkCommandBuffer);
    // void begin(VkCommandBuffer);
    // void end(VkCommandBuffer);
    // void update();

    // Context& context;
    // VkQueryPool queryPool = VK_NULL_HANDLE;
    // std::vector<uint64_t> stats;
private:
    void compile(MemoryBuffer);
    std::vector<ShaderProgramInfo> programs;
    std::vector<SvUbo> ubos;
};
} // namespace renderingEngine