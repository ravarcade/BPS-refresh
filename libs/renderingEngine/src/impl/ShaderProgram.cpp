#include "ShaderProgram.hpp"
#include <cstring>
#include "Context.hpp"
#include "RenderPass.hpp"
#include "shaders/ShaderCompiler.hpp"
#include "shaders/ShaderReflections.hpp"
#include "shaders/ShaderVariable.hpp"
#include "tools/to_vector.hpp"

namespace
{
using namespace renderingEngine;
constexpr auto sourceCode(const char* msg)
{
    auto begin = reinterpret_cast<const uint8_t*>(msg);
    return MemoryBuffer(begin, strlen(msg));
}

const auto test_vert = sourceCode(R"(
#version 450

layout (location = 0) in vec2 inPos;
layout (location = 1) in vec4 inColor;

layout (push_constant) uniform PushConstants {
	vec2 scale;
	vec2 translate;
} pushConstants;

layout (location = 0) out vec4 outColor;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

void main() 
{
	outColor = inColor;
	gl_Position = vec4(inPos * pushConstants.scale + pushConstants.translate, 0.0, 1.0);
}
)");

const auto test_frag = sourceCode(R"(
#version 450

layout (location = 0) in vec4 inColor;
layout (location = 0) out vec4 outColor;

void main() 
{
	outColor = inColor;
}
)");

std::unique_ptr<ShaderReflections> createReflections(std::vector<std::vector<uint8_t>> compiledShaders)
{
    std::unique_ptr<ShaderReflections> shaderReflections = std::make_unique<ShaderReflections>();
    for (const auto& shader : compiledShaders)
        shaderReflections->compile(shader);

    return shaderReflections;
}

ShaderProgram::Type getShaderProgramType(std::unique_ptr<ShaderReflections>& shaderReflections)
{
    if (shaderReflections->outputNames.size() == 1 && shaderReflections->outputNames[0] == "outColor")
    {
        return shaderReflections->vertexAttributes.size() == 0 ? ShaderProgram::Type::deferred_resolve
                                                               : ShaderProgram::Type::forward;
    }
    return ShaderProgram::Type::deferred;
}

std::vector<VkPipelineShaderStageCreateInfo> createShaderStages(
    Context& context,
    std::unique_ptr<ShaderReflections>& shaderReflections)
{
    return tools::to_vector(
        shaderReflections->shaderStageInfos,
        [&context](const auto& ssi)
        {
            VkPipelineShaderStageCreateInfo shaderStage = {};
            shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            shaderStage.stage = ssi.stage;
            shaderStage.pName = ssi.entryPointName.c_str();

            VkShaderModuleCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
            createInfo.codeSize = ssi.compiledShaderStage.size();
            createInfo.pCode = reinterpret_cast<const uint32_t*>(ssi.compiledShaderStage.data());
            if (vkCreateShaderModule(context.device, &createInfo, context.ire.allocator, &shaderStage.module) !=
                VK_SUCCESS)
            {
                throw std::runtime_error("failed to create shader module!");
            }
            return shaderStage;
        });
}

VertexAttributeBase getVABase(const VertexAttribute& val)
{
    return std::visit([](const auto& va) -> VertexAttributeBase { return va; }, val);
}
} // namespace

namespace renderingEngine
{
ShaderProgram::ShaderProgram(Context& context)
    : context{context}, shaderReflections{std::make_unique<ShaderReflections>()}
{
    ShaderCompiler sc;
    compiledShaders = {sc.compile(test_vert, ShaderStage::vertex), sc.compile(test_frag, ShaderStage::fragment)};

    createGraphicsPipeline();
}

ShaderProgram::~ShaderProgram()
{
    context.vkDestroy(pipeline);
}

void ShaderProgram::createGraphicsPipeline()
{
    context.vkDestroy(pipeline);
    shaderReflections = std::move(createReflections(compiledShaders));
    shaderProgramType = getShaderProgramType(shaderReflections);
    context.derreferedRenderPass->renderPass;
    auto shaderStages = createShaderStages(context, shaderReflections);
    auto vertexInputInfo = getVertexInputInfo();
}

VkPipelineVertexInputStateCreateInfo ShaderProgram::getVertexInputInfo()
{
    bindingDescription.clear();
    attributeDescriptions.clear();
    // auto& va = shaderReflections->vertexAttributes;

    // for (uint32_t binding = 0; binding < va.strides.size(); ++binding)
    // {
    //     auto stride = va.strides[binding];
    //     if (stride > 0)
    //     {
    //         m_bindingDescription.push_back({binding, stride, VK_VERTEX_INPUT_RATE_VERTEX});
    //     }
    // }

    for (VertexAttribute& attr : shaderReflections->vertexAttributes)
    {
        auto vab = getVABase(attr);
        // attr.
        // attributeDescriptions.push_back({vab.location, attr.binding, attr.format, attr.offset});
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();

    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return vertexInputInfo;
}
} // namespace renderingEngine
