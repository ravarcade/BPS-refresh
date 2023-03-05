#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "ShaderVariable.hpp"
#include "VertexAttribute.hpp"
#include "common/MemoryBuffer.hpp"
#include <fmt/ostream.h>

namespace renderingEngine
{
constexpr unsigned VULKAN_NUM_DESCRIPTOR_SETS = 4;
constexpr unsigned VULKAN_NUM_BINDINGS = 16;

struct Context;
struct ShaderStageInfo
{
    MemoryBuffer compiledShaderStage;
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
    ShaderReflections(){};
    ShaderReflections(std::vector<MemoryBuffer>&&);
    ~ShaderReflections();

    void compile(MemoryBuffer);
    // void reset(VkCommandBuffer);
    // void begin(VkCommandBuffer);
    // void end(VkCommandBuffer);
    // void update();

    // Context& context;
    // VkQueryPool queryPool = VK_NULL_HANDLE;
    // std::vector<uint64_t> stats;
    // private:
    std::vector<ShaderStageInfo> shaderStageInfos;
    std::vector<SvUbo> ubos;
    std::vector<SvPushConst> pushConstants;
    std::vector<SvSampler> samplers;
    std::vector<VertexAttribute> vertexAttributes;
    std::vector<std::string> outputNames;
    ResourceLayout resourceLayout;
};
} // namespace renderingEngine

template <> struct fmt::formatter<renderingEngine::ShaderReflections> : ostream_formatter {};