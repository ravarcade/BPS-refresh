#pragma once
#include <string>
#include <variant>
#include <vector>
#include <vulkan/vulkan.h>
#include "common/MemoryBuffer.hpp"
#include "spirv_cross/spirv_glsl.hpp"

namespace renderingEngine
{
class ShaderVariableBuilder
{
    using CompilerGLSL = SPIRV_CROSS_NAMESPACE::CompilerGLSL;
    using SPIRType = SPIRV_CROSS_NAMESPACE::SPIRType;

public:
    ShaderVariableBuilder(CompilerGLSL& compiler, const SPIRType& baseType, size_t idx)
        : compiler{compiler}, baseType{baseType}, idx{idx}
    {
    }

    template <typename T>
    T build()
    {
        auto typeId = baseType.member_types[idx];
        auto member_type = compiler.get_type(typeId);
        T t;
        t.name = compiler.get_member_name(baseType.self, idx);
        t.vecsize = member_type.vecsize;
        t.columns = member_type.columns;
        t.array.resize(member_type.array.size());
        for (uint32_t i = 0; i < t.array.size(); ++i)
        {
            t.array[i] = member_type.array[i];
        }
        t.offset = compiler.type_struct_member_offset(baseType, idx);
        t.size = static_cast<uint32_t>(compiler.get_declared_struct_member_size(baseType, idx));
        return t;
    }

    template <typename T>
    T buildWithMembers(std::vector<ShaderVariable> members)
    {
        T t = build<T>();
        t.members = members;
        return t;
    }

private:
    CompilerGLSL& compiler;
    const SPIRType& baseType;
    size_t idx;
};
} // namespace renderingEngine