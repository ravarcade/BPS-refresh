#pragma once
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace renderingEngine
{
struct Context;
struct ShaderReflections;

struct ShaderProgram
{
    enum class Type
    {
        forward,
        deferred,
        deferred_resolve
    };

    ShaderProgram(Context&);
    ~ShaderProgram();

    void createGraphicsPipeline();
    // void reset(VkCommandBuffer);
    // void begin(VkCommandBuffer);
    // void end(VkCommandBuffer);
    // void update();

    Context& context;
    std::unique_ptr<ShaderReflections> shaderReflections;
    std::vector<std::vector<uint8_t>> compiledShaders;
    // VkQueryPool queryPool = VK_NULL_HANDLE;
	// std::vector<uint64_t> stats;
private:
    VkPipelineVertexInputStateCreateInfo getVertexInputInfo();
    VkPipelineLayout getPipelineLayout();

	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
    VkPipeline pipeline = VK_NULL_HANDLE;
    Type shaderProgramType;
    std::vector<VkDescriptorSetLayout> descriptorSetLayout;
    std::vector<VkVertexInputBindingDescription> bindingDescription;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
};
} // namespace renderingEngine