#include "ShaderReflections.hpp"
// #include <stdexcept>
// #include "IRenderingEngine.hpp"
#include "Context.hpp"
#include "common/MemoryBuffer.hpp"
#include "spirv_cross/spirv_glsl.hpp"

namespace
{
using namespace SPIRV_CROSS_NAMESPACE;
using namespace renderingEngine;

static const unsigned VULKAN_NUM_DESCRIPTOR_SETS = 4;
static const unsigned VULKAN_NUM_BINDINGS = 16;
constexpr std::string_view SHAREDUBOTYPENAME = "SharedUBO";
constexpr bool ForceHostVisibleUBOS = true;

CompilerGLSL compile(MemoryBuffer source)
{
    return CompilerGLSL(reinterpret_cast<const uint32_t*>(source.data()), (source.size() + 3 / sizeof(uint32_t)));
}

ShaderProgramInfo createProgramInfo(CompilerGLSL& compiler)
{
    static VkShaderStageFlagBits executionModelToStage[] = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT,
        VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT,
        VK_SHADER_STAGE_GEOMETRY_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        VK_SHADER_STAGE_COMPUTE_BIT};
    auto entry_points = compiler.get_entry_points_and_stages();
    auto& entry_point = entry_points[0];
    return {entry_point.name, executionModelToStage[entry_point.execution_model]};
}

enum ShaderReflectionType {
	UNKNOWN = 0,
	Int32 = 1,
	UInt32 = 2,
	Int16 = 3,
	UInt16 = 4,
	Int8 = 5,
	UInt8 = 6,
	Float32 = 7,
	Vec2 = 8,
	Vec3 = 9,
	Vec4 = 10,
	Mat3 = 11,
	Mat4 = 12
};
/*
void getDetails(CompilerGLSL& comp, SPIRType& type, ValMemberDetails& ent)
{
    static ShaderReflectionType typeConv[] = {
        ShaderReflectionType::UNKNOWN, // BaseType::Unknown
        ShaderReflectionType::UNKNOWN, // BaseType::Void
        ShaderReflectionType::UNKNOWN, // BaseType::Boolean
        ShaderReflectionType::UNKNOWN, // BaseType::Char
        ShaderReflectionType::Int32, // BaseType::Int

        ShaderReflectionType::UInt32, // BaseType::UInt
        ShaderReflectionType::UNKNOWN, // BaseType::Int64
        ShaderReflectionType::UNKNOWN, // BaseType::UInt64
        ShaderReflectionType::UNKNOWN, // BaseType::AtomicCounter
        ShaderReflectionType::UNKNOWN, // BaseType::Half

        ShaderReflectionType::Float32, // BaseType::Float
        ShaderReflectionType::UNKNOWN, // BaseType::Double
        ShaderReflectionType::UNKNOWN, // BaseType::Struct (UBOs too)
        ShaderReflectionType::UNKNOWN, // BaseType::Image
        ShaderReflectionType::UNKNOWN, // BaseType::SampledImage

        ShaderReflectionType::UNKNOWN, // BaseType::Sampler
    };

    static uint32_t typeSize[] = {0, 0, 0, 1, 4, 4, 8, 8, 0, 2, 4, 8, 0, 0, 0, 0};

    uint32_t toPropType[] = {
        uint32_t(Property::PT_UNKNOWN),
        Property::PT_I32,
        Property::PT_U32,
        Property::PT_I16,
        Property::PT_U16,
        Property::PT_I8,
        Property::PT_U8,
        Property::PT_F32,

        Property::PT_F32, // Vec2,3,4
        Property::PT_F32,
        Property::PT_F32,

        Property::PT_F32, // Mat3, 4
        Property::PT_F32};

    uint32_t toPropCount[] = {
        0,
        1, // i32
        1, // u32
        1, // i16
        1, // u16
        1, // i8
        1, // u8
        1, // f32

        2, // vec2
        3, // vec3
        4, // vec4

        9, // mat3
        16, // mat4
    };

    ent.array = type.array.size() == 1 ? type.array[0] : 1;
    ent.vecsize = type.vecsize;
    ent.colsize = type.columns;

    if (type.basetype == SPIRType::BaseType::Struct && type.member_types.size())
    {
        // ent.size = static_cast<uint32_t>(comp.get_declared_struct_size(comp.get_type(res.base_type_id)));
        // ent.size = static_cast<uint32_t>(comp.get_declared_struct_size(comp.get_type(type.parent_type)));
        auto& members = type.member_types;
        for (uint32_t i = 0; i < members.size(); ++i)
        {
            SPIRType member_type = comp.get_type(members[i]);
            ValMemberDetails mem;
            mem.name = comp.get_member_name(type.self, i);
            mem.rootTypeName = comp.get_name(members[i]);
            mem.offset = comp.type_struct_member_offset(type, i);
            mem.size = static_cast<uint32_t>(comp.get_declared_struct_member_size(type, i));
            mem.array = !member_type.array.empty() ? member_type.array[0] : 1;
            mem.array_stride = !member_type.array.empty() ? comp.type_struct_member_array_stride(type, i) : 0;
            mem.vecsize = member_type.vecsize;
            mem.colsize = member_type.columns;
            mem.matrix_stride = comp.has_decoration(i, spv::DecorationMatrixStride)
                ? comp.type_struct_member_matrix_stride(type, i)
                : 0;
            mem.type = typeConv[member_type.basetype];
            SetSimplifiedType(mem);
            mem.propertyType = toPropType[mem.type];
            mem.propertyCount = toPropCount[mem.type];

            if (member_type.basetype == SPIRType::BaseType::Struct && member_type.member_types.size())
            {
                GetDetails(comp, member_type, mem);
            }

            // finetune for array of structs:
            if (mem.members.size() > 0 && mem.array > 0 &&
                mem.array_stride > 0) // it is array of struct! so mark it as array
            {
                mem.propertyType = Property::PT_ARRAY;
                mem.propertyCount = mem.array;
            }

            ent.members.push_back(mem);
        }
    }
    else if (typeConv[type.basetype] != ShaderReflectionType::UNKNOWN)
    {
        ent.type = typeConv[type.basetype];
        ent.size = typeSize[type.basetype] * ent.vecsize * ent.colsize * ent.array;
        SetSimplifiedType(ent);
    }
}

void getDetails(CompilerGLSL &comp, Resource &res, ValDetails &det)
{
	det = {};
	det.set = comp.has_decoration(res.id, spv::DecorationDescriptorSet) ? det.set = comp.get_decoration(res.id, spv::DecorationDescriptorSet) : 0;
	det.binding = comp.has_decoration(res.id, spv::DecorationBinding) ? comp.get_decoration(res.id, spv::DecorationBinding) : 0;

	det.entry.name = comp.get_name(res.id);
	det.entry.rootTypeName = comp.get_name(res.base_type_id);
	SPIRType type = comp.get_type(res.type_id);
	if (type.basetype == SPIRType::BaseType::Struct && type.member_types.size())
	{
		det.entry.size = static_cast<uint32_t>(comp.get_declared_struct_size(comp.get_type(res.base_type_id)));
	}
	getDetails(comp, type, det.entry);
}
*/
auto parseUniformBuffer(CompilerGLSL& compiler, Resource& buffer)
{
/*
    auto set = compiler.get_decoration(buffer.id, spv::DecorationDescriptorSet);
    auto binding = compiler.get_decoration(buffer.id, spv::DecorationBinding);

    ValDetails vd;
    getDetails(compiler, buffer, vd);
    vd.stage = prg.stage;
    vd.isSharedUBO = vd.entry.rootTypeName == SHAREDUBOTYPENAME;
    vd.isHostVisibleUBO = ForceHostVisibleUBOS || vd.isSharedUBO;

    bool isUniq = true;
    for (auto& vdPrev : m_params_in_ubos)
    {
        if (vdPrev.binding == vd.binding && vdPrev.set == vd.set && vdPrev.stage != vd.stage)
        {
            // same set binding
            vdPrev.stage |= vd.stage;
            isUniq = false;
            break;
        }
    }

    if (isUniq)
    {
        m_params_in_ubos.push_back(vd);

        if (vd.isSharedUBO)
        {
            m_shared_ubos_size += vd.entry.size;
        }
        else
        {
            m_total_ubos_size += vd.entry.size;

            if (m_ubo_sizes.size() <= vd.binding)
            {
                size_t i = m_ubo_sizes.size();
                m_ubo_sizes.resize(vd.binding + 1);
                for (; i < vd.binding; ++i)
                    m_ubo_sizes[i] = 0;
            }
            m_ubo_sizes[vd.binding] = vd.entry.size;
        }
    }

    auto name = compiler.get_name(buffer.id);
    const SPIRType& type = compiler.get_type(buffer.type_id);
    uint32_t descriptorCount = type.array.size() == 1 ? type.array[0] : 1;

    m_layout.descriptorSets[set].uniform_buffer_mask |= 1u << binding;
    m_layout.descriptorSets[set].descriptorCount[binding] = descriptorCount;
    m_layout.descriptorSets[set].stages[binding] |= prg.stage;
*/
}

auto parseUniformBuffers(CompilerGLSL& compiler)
{
    auto resources = compiler.get_shader_resources();
    for (auto buffer : resources.uniform_buffers)
    {
        parseUniformBuffer(compiler, buffer);
    }
}
}; // namespace

namespace renderingEngine
{
ShaderReflections::ShaderReflections(MemoryBuffer source)
{
    // auto& device = context.device;
    // auto& allocator = context.ire.allocator;
    // VkSemaphoreCreateInfo semaphoreInfo = {};
    // semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    // if (vkCreateSemaphore(device, &semaphoreInfo, allocator, &imageAvailableSemaphore) != VK_SUCCESS ||
    //     vkCreateSemaphore(device, &semaphoreInfo, allocator, &renderFinishedSemaphore) != VK_SUCCESS)
    // {
    // 	throw std::runtime_error("failed to create semaphores!");
    // }
    compile(source);
}

ShaderReflections::~ShaderReflections()
{
    // context.vkDestroy(renderFinishedSemaphore);
    // context.vkDestroy(imageAvailableSemaphore);
}

void ShaderReflections::compile(MemoryBuffer source)
{
    CompilerGLSL compiler = ::compile(source);
    auto constants = compiler.get_specialization_constants();
    auto resources = compiler.get_shader_resources();
    auto pi = createProgramInfo(compiler);
    parseUniformBuffers(compiler);
    // auto entry_points = compiled.get_entry_points_and_stages();
    // auto &entry_point = entry_points[0];
}
} // namespace renderingEngine
