#pragma once
#include <vulkan/vulkan.h>
#include <vector>

namespace renderingEngine
{
struct Context;

struct ShaderProgram
{
    ShaderProgram(Context&);
    ~ShaderProgram();

    // void reset(VkCommandBuffer);
    // void begin(VkCommandBuffer);
    // void end(VkCommandBuffer);
    // void update();

    Context& context;
    // VkQueryPool queryPool = VK_NULL_HANDLE;
	// std::vector<uint64_t> stats;
};
} // namespace renderingEngine