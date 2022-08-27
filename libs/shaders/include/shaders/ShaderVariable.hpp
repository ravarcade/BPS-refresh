#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
struct SvBase
{
    std::string name;
    uint32_t vecsize{1};
    uint32_t columns{1};
    std::vector<uint32_t> array{};
    uint32_t offset;
    uint32_t size;
};

struct SvFloat : SvBase
{
};

struct SvInt : SvBase
{
};

struct SvUbo;
struct SvStruct;
struct SvPushConst;
using ShaderVariable = std::variant<SvUbo, SvPushConst, SvStruct, SvFloat, SvInt>;

struct SvUbo
{
    uint32_t set{0};
    uint32_t binding{0};
    uint32_t stage = VK_SHADER_STAGE_VERTEX_BIT;
    std::string name;
    std::string rootTypeName;
    uint32_t offset;
    uint32_t size;
    bool isSharedUBO = false;
    bool isHostVisibleUBO = false;
    std::vector<ShaderVariable> members;
};

struct SvPushConst
{
    uint32_t set{0};
    uint32_t binding{0};
    std::string name;
    std::string rootTypeName;
    uint32_t offset;
    uint32_t size;
    std::vector<ShaderVariable> members;
};

struct SvStruct : SvBase
{
    std::vector<ShaderVariable> members;
};

std::ostream& operator<<(std::ostream& out, const ShaderVariable& val);
} // namespace renderingEngine