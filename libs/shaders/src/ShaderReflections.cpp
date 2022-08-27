#include "shaders/ShaderReflections.hpp"
#include <ranges>
#include "ShaderVariableBuilder.hpp"
#include "common/Logger.hpp"
#include "common/MemoryBuffer.hpp"
#include "shaders/ShaderVariable.hpp"
#include "spirv_cross/spirv_glsl.hpp"

namespace
{
using namespace SPIRV_CROSS_NAMESPACE;
using namespace renderingEngine;
// using std::ranges::views;

constexpr std::string_view SHAREDUBOTYPENAME = "SharedUBO";
constexpr bool ForceHostVisibleUBOS = true;

CompilerGLSL compile(MemoryBuffer source)
{
    return CompilerGLSL(reinterpret_cast<const uint32_t*>(source.data()), (source.size() + 3) / sizeof(uint32_t));
}

// ShaderProgramInfo createProgramInfo(CompilerGLSL& compiler)
// {
//     static VkShaderStageFlagBits executionModelToStage[] = {
//         VK_SHADER_STAGE_VERTEX_BIT,
//         VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
//         VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
//         VK_SHADER_STAGE_GEOMETRY_BIT,
//         VK_SHADER_STAGE_FRAGMENT_BIT,
//         VK_SHADER_STAGE_COMPUTE_BIT};
//     auto entry_points = compiler.get_entry_points_and_stages();
//     auto& entry_point = *entry_points.begin();
//     return {entry_point.name, executionModelToStage[entry_point.execution_model]};
// }
/*
class SvBuilder
{
public:
    SvBuilder(CompilerGLSL& compiler, const SPIRType& baseType, uint32_t idx)
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
    uint32_t idx;
};
*/
class SrUniformBuffers
{
public:
    SrUniformBuffers(CompilerGLSL& compiler) : compiler{compiler} {}

    std::vector<SvUbo> parseUniformBuffers()
    {
        std::vector<SvUbo> ubos;
        for (auto& uniform_buffer : compiler.get_shader_resources().uniform_buffers)
        {
            ubos.push_back(parseUniformBuffer(uniform_buffer));
        }
        return ubos;
    }

    SvUbo parseUniformBuffer(Resource& res)
    {
        auto set = compiler.has_decoration(res.id, spv::DecorationDescriptorSet)
            ? compiler.get_decoration(res.id, spv::DecorationDescriptorSet)
            : 0;
        auto binding = compiler.has_decoration(res.id, spv::DecorationBinding)
            ? compiler.get_decoration(res.id, spv::DecorationBinding)
            : 0;
        auto name = compiler.get_name(res.id);
        auto rootTypeName = compiler.get_name(res.base_type_id);
        auto offset = 0u;
        auto size = static_cast<uint32_t>(compiler.get_declared_struct_size(compiler.get_type(res.base_type_id)));
        auto isSharedUBO = rootTypeName == SHAREDUBOTYPENAME;
        auto isHostVisibleUBO = isSharedUBO || ForceHostVisibleUBOS;
        auto members = getStructMembers(compiler.get_type(res.type_id));
        return {
            set,
            binding,
            VK_SHADER_STAGE_VERTEX_BIT,
            name,
            rootTypeName,
            offset,
            size,
            isSharedUBO,
            isHostVisibleUBO,
            members};
    }

    std::vector<ShaderVariable> getStructMembers(const SPIRType& type)
    {
        std::vector<ShaderVariable> members;
        for (uint32_t i = 0; i < type.member_types.size(); ++i)
        {
            members.push_back(getStructMember(type, i));
        }
        return members;
    }

    ShaderVariable getStructMember(const SPIRType& baseType, uint32_t idx)
    {
        ShaderVariableBuilder svb(compiler, baseType, idx);
        auto typeId = baseType.member_types[idx];
        auto member_type = compiler.get_type(typeId);
        auto member_name = compiler.get_member_name(baseType.self, idx);

        switch (member_type.basetype)
        {
            case SPIRType::BaseType::Float:
                return svb.build<SvFloat>();
            case SPIRType::BaseType::Int:
                return svb.build<SvInt>();
            case SPIRType::BaseType::Struct:
                return svb.buildWithMembers<SvStruct>(getStructMembers(member_type));
            default:
                log_err("unsuported data type: {} for {}", member_type.basetype, member_name);
        }

        return {};
    }

    std::vector<SvPushConst> parsePushConstants()
    {
        std::vector<SvPushConst> pushConstants;
        for (auto& pushConst : compiler.get_shader_resources().push_constant_buffers)
        {
            pushConstants.push_back(parsePushConstant(pushConst, pushConstants));
        }
        return pushConstants;
    }

    SvPushConst parsePushConstant(Resource& res, std::vector<SvPushConst>& pushConstants)
    {
        auto set = compiler.has_decoration(res.id, spv::DecorationDescriptorSet)
            ? compiler.get_decoration(res.id, spv::DecorationDescriptorSet)
            : 0;
        auto binding = compiler.has_decoration(res.id, spv::DecorationBinding)
            ? compiler.get_decoration(res.id, spv::DecorationBinding)
            : 0;
        auto name = compiler.get_name(res.id);
        auto rootTypeName = compiler.get_name(res.base_type_id);
        auto offset = 0u;
        auto size = static_cast<uint32_t>(compiler.get_declared_struct_size(compiler.get_type(res.base_type_id)));

        auto members = getStructMembers(compiler.get_type(res.type_id));

        return SvPushConst{set, binding, name, rootTypeName, offset, size, members};
    }

private:
    CompilerGLSL& compiler;
};
}; // namespace

namespace renderingEngine
{
ShaderReflections::ShaderReflections(MemoryBuffer vert, MemoryBuffer frag)
{
    compile(vert, frag);
    // auto& device = context.device;
    // auto& allocator = context.ire.allocator;
    // VkSemaphoreCreateInfo semaphoreInfo = {};
    // semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // if (vkCreateSemaphore(device, &semaphoreInfo, allocator, &imageAvailableSemaphore) != VK_SUCCESS ||
    //     vkCreateSemaphore(device, &semaphoreInfo, allocator, &renderFinishedSemaphore) != VK_SUCCESS)
    // {
    // 	throw std::runtime_error("failed to create semaphores!");
    // }
}

ShaderReflections::~ShaderReflections()
{
    // context.vkDestroy(renderFinishedSemaphore);
    // context.vkDestroy(imageAvailableSemaphore);
}

void ShaderReflections::compile(MemoryBuffer vert, MemoryBuffer /*frag*/)
{
    CompilerGLSL compiler = ::compile(vert);
    SrUniformBuffers sr{compiler};
    ubos = sr.parseUniformBuffers();
    pushConstants = sr.parsePushConstants();

    // auto constants = compiler.get_specialization_constants();
    // auto resources = compiler.get_shader_resources();
    // auto pi = createProgramInfo(compiler);
    // parseUniformBuffers(compiler);
    // auto entry_points = compiled.get_entry_points_and_stages();
    // auto &entry_point = entry_points[0];
    log_inf("ubos: {}", ubos);
    log_inf("pushConstants: {}", pushConstants);
}
} // namespace renderingEngine
