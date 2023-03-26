#include "shaders/ShaderReflections.hpp"
#include <ranges>
#include <map>
#include "ShaderVariableBuilder.hpp"
#include "common/Logger.hpp"
#include "common/MemoryBuffer.hpp"
#include "shaders/ShaderVariable.hpp"
#include "spirv_cross/spirv_glsl.hpp"
#include "tools/to_vector.hpp"

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

uint32_t executionModelToStage(spv::ExecutionModel em)
{
    static std::unordered_map<spv::ExecutionModel, uint32_t> executionModelMap = {
        {spv::ExecutionModel::ExecutionModelVertex, VK_SHADER_STAGE_VERTEX_BIT},
        {spv::ExecutionModel::ExecutionModelTessellationControl, VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT},
        {spv::ExecutionModel::ExecutionModelTessellationEvaluation, VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT},
        {spv::ExecutionModel::ExecutionModelGeometry, VK_SHADER_STAGE_GEOMETRY_BIT},
        {spv::ExecutionModel::ExecutionModelFragment, VK_SHADER_STAGE_FRAGMENT_BIT},
        {spv::ExecutionModel::ExecutionModelGLCompute, VK_SHADER_STAGE_COMPUTE_BIT},
    };
    assert(executionModelMap.find(em) != executionModelMap.end());
    return executionModelMap[em];
}

class ShaderReflectionsImpl
{
public:
    ShaderReflectionsImpl(CompilerGLSL& compiler, ResourceLayout& resourceLayout)
        : compiler{compiler}, resourceLayout{resourceLayout}
    {
        auto entry_points = compiler.get_entry_points_and_stages();
        auto& entry_point = entry_points[0];
        entryPointName = entry_point.name;
        stage = executionModelToStage(entry_point.execution_model);
        resources = compiler.get_shader_resources();
    }

    void parseUniformBuffers(std::vector<SvUbo>& ubos)
    {
        for (auto& uniform_buffer : resources.uniform_buffers)
        {
            auto svUbo = parseUniformBuffer(uniform_buffer);
            ubos.push_back(svUbo);

            const SPIRType &type = compiler.get_type(uniform_buffer.type_id);
			uint32_t descriptorCount = type.array.size() == 1 ? type.array[0] : 1;
            resourceLayout.descriptorSets[svUbo.set].uniform_buffer_mask |= 1u << svUbo.binding;
            resourceLayout.descriptorSets[svUbo.set].descriptorCount[svUbo.binding] = descriptorCount;
            resourceLayout.descriptorSets[svUbo.set].stages[svUbo.binding] |= svUbo.stage;
        }
    }

    template <typename T>
    T createSv(Resource& res)
    {
        T sv;
        sv.set = compiler.has_decoration(res.id, spv::DecorationDescriptorSet)
            ? compiler.get_decoration(res.id, spv::DecorationDescriptorSet)
            : 0;
        sv.binding = compiler.has_decoration(res.id, spv::DecorationBinding)
            ? compiler.get_decoration(res.id, spv::DecorationBinding)
            : 0;
        sv.name = compiler.get_name(res.id);
        sv.rootTypeName = compiler.get_name(res.base_type_id);
        sv.offset = 0u;
        sv.size = static_cast<uint32_t>(compiler.get_declared_struct_size(compiler.get_type(res.base_type_id)));

        sv.members = getStructMembers(compiler.get_type(res.type_id));
        return sv;
    }

    SvUbo parseUniformBuffer(Resource& res)
    {
        auto sv = createSv<SvUbo>(res);
        sv.isSharedUBO = sv.rootTypeName == SHAREDUBOTYPENAME;
        sv.isHostVisibleUBO = sv.isSharedUBO || ForceHostVisibleUBOS;
        sv.stage = stage;
        return sv;
    }

    SvPushConst parsePushConstant(Resource& res)
    {
        auto sv = createSv<SvPushConst>(res);
        sv.stage = stage;
        return sv;
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

    ShaderVariable getStructMember(const SPIRType& baseType, size_t idx)
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

    void parsePushConstants(std::vector<SvPushConst>& pushConstants)
    {
        for (auto& pushConst : resources.push_constant_buffers)
        {
            pushConstants.push_back(parsePushConstant(pushConst));
        }
    }

    void parseSamplers(std::vector<SvSampler>& samplers)
    {
        for (auto& resource : resources.sampled_images)
        {
            auto set = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
            auto binding = compiler.get_decoration(resource.id, spv::DecorationBinding);
            auto name = compiler.get_name(resource.id);
            const SPIRType& type = compiler.get_type(resource.type_id);
            assert(type.basetype == SPIRType::SampledImage);
            if (type.basetype == SPIRType::SampledImage)
            {
                uint32_t descriptorCount = type.array.size() == 1 ? type.array[0] : 1;
                samplers.push_back({set, binding, name, static_cast<uint32_t>(type.image.dim), stage});
                resourceLayout.descriptorSets[set].sampled_image_mask |= 1u << binding;
                resourceLayout.descriptorSets[set].descriptorCount[binding] = descriptorCount;
                resourceLayout.descriptorSets[set].stages[binding] |= stage;
            }
        }
    }

    static VertexAttributeBase getVABase(const VertexAttribute& val)
    {
        return std::visit([](const auto& va) -> VertexAttributeBase { return va; }, val);
    }

    void sortVertexAttributes(std::vector<VertexAttribute>& vertexAttribs)
    {
        std::sort(
            vertexAttribs.begin(),
            vertexAttribs.end(),
            [](const auto& a, const auto& b)
            {
                auto _a = getVABase(a);
                auto _b = getVABase(b);
                return _a.binding == _b.binding ? _a.location < _b.location : _a.binding < _b.binding;
            });
    }

    void parseVertexAttribs(std::vector<VertexAttribute>& vertexAttribs)
    {
        if (stage == VK_SHADER_STAGE_VERTEX_BIT)
        {
            for (auto attrib : resources.stage_inputs)
            {
                vertexAttribs.push_back(getVertexAttrib(attrib));
            }
        }
        sortVertexAttributes(vertexAttribs);
    }

    template <typename T>
    VertexAttribute createVertexAttribute(Resource& attrib)
    {
        return T{
            compiler.get_decoration(attrib.id, spv::DecorationBinding),
            compiler.get_decoration(attrib.id, spv::DecorationLocation),
            compiler.get_name(attrib.id),
            compiler.get_type(attrib.type_id).vecsize};
    }

    VertexAttribute getVertexAttrib(Resource& attrib)
    {
        const SPIRType& type = compiler.get_type(attrib.type_id);
        switch (type.basetype)
        {
            case SPIRType::BaseType::Float:
                return createVertexAttribute<VaFloat>(attrib);
            case SPIRType::BaseType::Int:
                return createVertexAttribute<VaInt>(attrib);
            default:
                log_err("unsuported data type: {} for {}", type.basetype, compiler.get_name(attrib.id));
        }
        return {};
    }

    void parseOutputNames(std::vector<std::string>& outputNames)
    {
        if (stage == VK_SHADER_STAGE_FRAGMENT_BIT)
        {
            for (uint32_t i = 0; i < resources.stage_outputs.size(); ++i)
            {
                outputNames.push_back(compiler.get_name(resources.stage_outputs[i].id));
            }
        }
    }

    ShaderStageInfo getShaderStageInfo(MemoryBuffer program)
    {
        return ShaderStageInfo{program, entryPointName, static_cast<VkShaderStageFlagBits>(stage)};
    }

private:
    CompilerGLSL& compiler;
    ResourceLayout& resourceLayout;
    std::string entryPointName;
    uint32_t stage;
    ShaderResources resources;
};

std::vector<VkPushConstantRange> mergePushConstants(const std::vector<SvPushConst>& pushConstants)
{
    std::map<std::pair<uint32_t, uint32_t>, VkPushConstantRange> mapPushConstantRangesInSets;
    for (const auto& svPushConst : pushConstants)
    {
        std::pair<uint32_t, uint32_t> key = {svPushConst.set, svPushConst.binding};
        auto it = mapPushConstantRangesInSets.find(key);
        if (it == mapPushConstantRangesInSets.end())
        {
            mapPushConstantRangesInSets.insert({key, {svPushConst.stage, svPushConst.offset, svPushConst.size}});
        }
        else
        {
            auto& pc = it->second;
            auto offset = std::min(pc.offset, svPushConst.offset);
            auto size = std::max(pc.offset + pc.size, svPushConst.offset + svPushConst.size) - offset;
            pc.offset = offset;
            pc.size = size;
            pc.stageFlags |= svPushConst.stage;
        }
    }

    return tools::to_vector(mapPushConstantRangesInSets, [](const auto& it) { return it.second; });
}

template <typename T>
bool isEmpty(const T& arr)
{
    return std::all_of(std::begin(arr), std::end(arr), [](const auto& item) { return item == 0; });
}

bool isEmpty(const ResourceLayout::DescriptorSetLayout& in)
{
    return in.uniform_buffer_mask == 0 and in.sampled_image_mask == 0 and isEmpty(in.descriptorCount) and
        isEmpty(in.stages);
}

template<typename T>
auto asMap(const T& arr)
{
    int i = 0;
    std::map<int, int> map;
    std::ranges::for_each(arr, [&i, &map](const auto it) mutable {
        if (it != 0)
        {
            map.insert(std::make_pair(i++, it));
        }
    });
    return map;
}
}; // namespace

namespace renderingEngine
{
ShaderReflections::ShaderReflections(std::vector<MemoryBuffer>&& programs)
{
    for (auto& program : programs)
    {
        compile(program);
    }
    parseReqults();
}

ShaderReflections::~ShaderReflections() {}

void ShaderReflections::compile(MemoryBuffer program)
{
    CompilerGLSL compiler = ::compile(program);
    ShaderReflectionsImpl sri{compiler, resourceLayout};
    sri.parseUniformBuffers(ubos);
    sri.parsePushConstants(pushConstants);
    sri.parseSamplers(samplers);
    sri.parseVertexAttribs(vertexAttributes);
    sri.parseOutputNames(outputNames);
    shaderStageInfos.push_back(sri.getShaderStageInfo(program));
}

void ShaderReflections::parseReqults()
{
    resourceLayout.pushConstants = mergePushConstants(pushConstants);
    log_inf("ubos: {}", ubos);
    log_inf("pushConstants: {}", pushConstants);
    log_inf("samplers: {}", samplers);
    log_inf("vertexAttribs: {}", vertexAttributes);
    log_inf("outputNames: {}", outputNames);
    log_inf("resourceLayout: {}", resourceLayout);
}

std::ostream& operator<<(std::ostream& out, const ResourceLayout::DescriptorSetLayout& in)
{
    if (isEmpty(in))
    {
        return out << "[empty]";
    }
    return out << fmt::format("[uniform_buffer_mask: {}, sampled_image_mask: {}, descriptorCount: {}, stages: {}]", in.uniform_buffer_mask, in.sampled_image_mask, asMap(in.descriptorCount), asMap(in.stages));
}

std::ostream& operator<<(std::ostream& out, const ResourceLayout& in)
{
    return out << fmt::format("descriptorSets: {}", in.descriptorSets);
}
} // namespace renderingEngine
