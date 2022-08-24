#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
struct Context;
struct ShaderProgramInfo
{
    // BAMS::CResRawData resource;
    std::string entryPointName;
    VkShaderStageFlagBits stage;
};

struct ValMemberDetails
{
    std::string name = "";
    std::string rootTypeName = "";
    uint32_t type = 0; // int/float/...
    uint32_t vecsize = 0; // num in row (1 - scaler, 2~4 vec2~vec4,...)
    uint32_t colsize = 0; // matrix columns
    uint32_t matrix_stride = 0;
    uint32_t array = 0;
    uint32_t array_stride;
    uint32_t location = 0;
    uint32_t offset = 0;
    uint32_t size = 0;
    std::vector<ValMemberDetails> members;
    uint32_t propertyType = 0;
    uint32_t propertyCount = 0;
};

struct ValDetails
{
    uint32_t set = 0;
    uint32_t binding = 0;
    uint32_t stage = VK_SHADER_STAGE_VERTEX_BIT;
    bool isSharedUBO = false;
    bool isHostVisibleUBO = false;
    ValMemberDetails entry;
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
};
} // namespace renderingEngine