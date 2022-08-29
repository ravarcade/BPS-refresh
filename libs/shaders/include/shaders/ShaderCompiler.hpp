#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"

namespace renderingEngine
{
enum class Shader
{
    vertex,
    fragment
    // add more later
};

class ShaderCompiler
{
public:
    ShaderCompiler();
    ~ShaderCompiler();

    std::vector<uint8_t> compile(MemoryBuffer sourceCode, Shader shader = Shader::vertex);

private:
    bool isIntialized{false};
};
} // namespace renderingEngine