#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
struct SvFloat
{
    std::string name;
    uint32_t vecsize{1};
    uint32_t columns{1};
};

struct SvInt
{
    std::string name;
    uint32_t vecsize{1};
    uint32_t columns{1};
};

struct SvUbo;
struct SvStruct;
using ShaderVariable = std::variant<SvUbo, SvStruct, SvFloat, SvInt>;

struct SvStruct
{
    std::string name;
    uint32_t vecsize{1};
    uint32_t columns{1};

    // StructDetails details;
    std::vector<ShaderVariable> members;
};

struct SvUbo
{
    uint32_t set{0};
    uint32_t binding{0};
    uint32_t stage = VK_SHADER_STAGE_VERTEX_BIT;
    std::string name;
    std::string rootTypeName;
    bool isSharedUBO = false;
    bool isHostVisibleUBO = false;
    std::vector<ShaderVariable> members;
};

std::ostream& operator<<(std::ostream& out, const ShaderVariable& val);
} // namespace renderingEngine