#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
class ShaderCompiler
{
public:
    ShaderCompiler();
    ~ShaderCompiler();

    std::vector<uint8_t> compile(MemoryBuffer sourceCode);

private:
    bool isIntialized{false};
};
} // namespace renderingEngine