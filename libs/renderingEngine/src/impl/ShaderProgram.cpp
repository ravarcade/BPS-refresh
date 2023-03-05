#include "ShaderProgram.hpp"
#include <cstring>
#include <unordered_map>
#include "Context.hpp"
#include "RenderPass.hpp"
#include "SwapChain.hpp"
#include "common/Logger.hpp"
#include "shaders/ShaderCompiler.hpp"
#include "shaders/ShaderReflections.hpp"
#include "shaders/ShaderVariable.hpp"
#include "tools/to_vector.hpp"
#include "DescriptorSetManager.hpp"
#include "tools/writeFile.hpp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
namespace std
{
template <>
struct hash<std::pair<std::string, uint32_t>>
{
    auto operator()(const std::pair<std::string, uint32_t>& pair) const -> size_t
    {
        return hash<std::string>()(pair.first) ^ hash<uint32_t>()(pair.second);
    }
};
} // namespace std

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

layout (location = 0) in vec4 outColor;
layout (location = 0) out vec4 outGuiColor;

void main() 
{
	outGuiColor = outColor;
}
)");

std::unique_ptr<ShaderReflections> createReflections(const std::vector<std::vector<uint8_t>> &compiledShaders)
{
    std::unique_ptr<ShaderReflections> shaderReflections = std::make_unique<ShaderReflections>();
    for (const auto& shader : compiledShaders)
	{
        shaderReflections->compile(shader);
	}

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
    ShaderReflections& shaderReflections)
{
    return tools::to_vector(
        shaderReflections.shaderStageInfos,
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

typedef std::pair<std::string, uint32_t> stringSizePair;

std::unordered_map<stringSizePair, VkFormat> formatSelector = {
    {{"inPos", 2}, VK_FORMAT_R32G32_SFLOAT},
    {{"inPos", 3}, VK_FORMAT_R32G32B32_SFLOAT},
    {{"inColor", 3}, VK_FORMAT_R8G8B8_UNORM},
    {{"inColor", 4}, VK_FORMAT_R8G8B8A8_UNORM},
};

std::unordered_map<VkFormat, uint32_t> vkFormatToSize = {
    {VK_FORMAT_R32G32_SFLOAT, 2 * sizeof(float)},
    {VK_FORMAT_R32G32B32_SFLOAT, 3 * sizeof(float)},
    {VK_FORMAT_R32G32B32A32_SFLOAT, 4 * sizeof(float)},
    {VK_FORMAT_R8G8B8_UNORM, 1 * sizeof(uint32_t)},
    {VK_FORMAT_R8G8B8A8_UNORM, 1 * sizeof(uint32_t)},
};

VkVertexInputAttributeDescription getAttribDesc(const VertexAttribute& attr, uint32_t& stride)
try
{
    auto va = getVABase(attr);
    auto format = formatSelector.at({va.name, va.vecsize});
    auto offset = stride;
    stride += vkFormatToSize.at(format);
    return {va.location, va.binding, format, offset};
}
catch (const std::out_of_range&)
{
    log_err("unknow vertex format for VertexAttribute: {}", attr);
    throw;
}

VkPipelineInputAssemblyStateCreateInfo getInputAssembly()
{
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO};
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    return inputAssembly;
}

VkPipelineViewportStateCreateInfo getViewportState(Context& context)
{
    const auto& swapChainExtent = context.swapChain->extent;
    static VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapChainExtent.width;
    viewport.height = (float)swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    // no scisors
    static VkRect2D scissor = {{0, 0}, swapChainExtent};

    // viewport and scisors in one structure: viewportState
    static VkPipelineViewportStateCreateInfo viewportState = {VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO};
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    return viewportState;
}

VkPipelineRasterizationStateCreateInfo getRasterizationState()
{
	VkPipelineRasterizationStateCreateInfo rasterizer = { VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional
	return rasterizer;
}

VkPipelineMultisampleStateCreateInfo getMultisampleState(VkSampleCountFlagBits  msaaSamples)
{
	VkPipelineMultisampleStateCreateInfo multisampling = { VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = msaaSamples;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	return multisampling;
}

VkPipelineDepthStencilStateCreateInfo getDepthStencilState()
{
	VkPipelineDepthStencilStateCreateInfo depthStencil = { VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f; // Optional
	depthStencil.maxDepthBounds = 1.0f; // Optional
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {}; // Optional
	depthStencil.back = {}; // Optional

	return depthStencil;
}

VkPipelineColorBlendStateCreateInfo getColorBlendState(const std::vector<std::string>& outputNames)
{
	static std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
	colorBlendAttachments.clear();

	static VkPipelineColorBlendAttachmentState noBlending =
	{
		VK_FALSE,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	static VkPipelineColorBlendAttachmentState blending =
	{
		VK_TRUE,
		VK_BLEND_FACTOR_SRC_ALPHA,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		VK_BLEND_FACTOR_ZERO,
		VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};


	for (auto &name : outputNames)
	{
		if (name == "outGuiColor")
		{
			colorBlendAttachments.push_back(blending);
		}
		else
		{
			colorBlendAttachments.push_back(noBlending);
		}
	}

	/* alpha blending
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
	*/

	VkPipelineColorBlendStateCreateInfo colorBlending = { VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
	colorBlending.flags = 0;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
	colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
	colorBlending.pAttachments = colorBlendAttachments.data();
	colorBlending.blendConstants[0] = 0.0f; // Optional
	colorBlending.blendConstants[1] = 0.0f; // Optional
	colorBlending.blendConstants[2] = 0.0f; // Optional
	colorBlending.blendConstants[3] = 0.0f; // Optional

	return colorBlending;
}

VkPipelineDynamicStateCreateInfo getDynamicState()
{
	// right now we don't set viewport and scissors in pipline creation
	static std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
	dynamicState.pDynamicStates = dynamicStates.data();

	return dynamicState;
}

VkRenderPass getRenderPass(Context& context, ShaderProgram::Type shaderProgramType)
{
    switch(shaderProgramType)
    {
        case ShaderProgram::Type::forward: return context.forwardRenderPass->renderPass;
        case ShaderProgram::Type::deferred: return context.derreferedRenderPass->renderPass;
        case ShaderProgram::Type::deferred_resolve: return context.derreferedRenderPass->renderPass; // bad choice!
    }
    return context.forwardRenderPass->renderPass;
}
} // namespace

namespace renderingEngine
{
ShaderProgram::ShaderProgram(Context& context)
    : context{context}, shaderReflections{std::make_unique<ShaderReflections>()}
{
    ShaderCompiler sc;
    compiledShaders = {sc.compile(test_vert, ShaderStage::vertex), sc.compile(test_frag, ShaderStage::fragment)};
	tools::writeFile("myVert.spv", compiledShaders[0]);
	tools::writeFile("myFrag.spv", compiledShaders[1]);
	// compiledShaders[0] = tools::loadFile("vert.spv");
	// compiledShaders[1] = tools::loadFile("frag.spv");

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
    auto shaderStages = createShaderStages(context, *shaderReflections);
    auto vertexInputInfo = getVertexInputInfo();
    auto inputAssembly = getInputAssembly();
    auto viewportState = getViewportState(context);
	auto rasterizer = getRasterizationState();
	auto multisampling = getMultisampleState(context.msaaSamples);
	auto depthStencil = getDepthStencilState();
	auto colorBlending = getColorBlendState(shaderReflections->outputNames);
	auto dynamicState = getDynamicState();
	auto pipelineLayout = getPipelineLayout();
	auto renderPass = getRenderPass(context, shaderProgramType);

    rasterizer.cullMode = VK_CULL_MODE_NONE; // for imGui
    depthStencil.depthTestEnable = VK_FALSE;
	depthStencil.depthWriteEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo pipelineInfo = { VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
	pipelineInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
	pipelineInfo.basePipelineIndex = -1; // Optional

    
#ifdef _DEBUG
	for (uint32_t i = 0; i < pipelineInfo.pVertexInputState->vertexAttributeDescriptionCount; ++i)
	{
		if (!vk->IsBufferFeatureSupported(pipelineInfo.pVertexInputState->pVertexAttributeDescriptions[i].format, VK_FORMAT_FEATURE_VERTEX_BUFFER_BIT))
		{
			throw std::runtime_error("Failed to create graphics pipeline! Attrib vertex fromat is not supported.");
		}
	}
#endif

	if (vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &pipelineInfo, context.ire.allocator, &pipeline) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create graphics pipeline!");
	}

	// some cleanups
	for (auto shaderStage : shaderStages)
        context.vkDestroy(shaderStage.module);

	// other stuffs use by shader program
	// _CreateUniformBuffers();
	// _BuildShaderDataBuffers();
}

VkPipelineVertexInputStateCreateInfo ShaderProgram::getVertexInputInfo()
{
    uint32_t stride = 0;
    attributeDescriptions = tools::to_vector(
        shaderReflections->vertexAttributes, [&stride](const auto& attr) { return getAttribDesc(attr, stride); });

    bindingDescription.clear();
    bindingDescription.push_back({attributeDescriptions[0].binding, stride, VK_VERTEX_INPUT_RATE_VERTEX});

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
    vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();

    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    return vertexInputInfo;
}

VkPipelineLayout ShaderProgram::getPipelineLayout()
{
    if (pipelineLayout)
        return pipelineLayout;
    auto& dsm = context.descriptorSetManager;
	
	// auto layout = shaderReflections.GetLayout();
	descriptorSetLayout = dsm->createDescriptorSetLayouts(*shaderReflections);
	// auto descriptorsRequirments = dsm->createDescriptorRequirments(*shaderReflections);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = { VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
	pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
	pipelineLayoutInfo.pSetLayouts = descriptorSetLayout.data();
	// pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(layout.pushConstants.size());
	// pipelineLayoutInfo.pPushConstantRanges = layout.pushConstants.data();

	if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, context.ire.allocator, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
	return pipelineLayout;
}
} // namespace renderingEngine
#pragma GCC diagnostic pop