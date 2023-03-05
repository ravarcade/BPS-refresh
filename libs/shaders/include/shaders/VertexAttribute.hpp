#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"
#include <fmt/ostream.h>

namespace renderingEngine
{
struct VertexAttributeBase
{
    uint32_t binding{0};
    uint32_t location{0};
    std::string name;
    uint32_t vecsize{1};
};

struct VaFloat : VertexAttributeBase
{
};

struct VaInt : VertexAttributeBase
{
};

using VertexAttribute = std::variant<VaFloat, VaInt>;

std::ostream& operator<<(std::ostream& out, const VertexAttribute& val);
} // namespace renderingEngine

template <> struct fmt::formatter<renderingEngine::VertexAttribute> : ostream_formatter {};