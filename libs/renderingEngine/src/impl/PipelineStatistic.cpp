#include "PipelineStatistic.hpp"
#include "Context.hpp"
#include "tools/countBits.hpp"

namespace renderingEngine
{
PipelineStatistic::PipelineStatistic(Context& context) : context{context}
{
	queryPool = VK_NULL_HANDLE;
	if (not context.ire.enablePiplineStatistic())
		return;

	VkQueryPoolCreateInfo queryPoolInfo = {};
	queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
	// This query pool will store pipeline statistics
	queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
	// Pipeline counters to be returned for this pool
	queryPoolInfo.pipelineStatistics =
		VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
		VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
		VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

	if (false)
		queryPoolInfo.pipelineStatistics |=
		VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_CONTROL_SHADER_PATCHES_BIT |
		VK_QUERY_PIPELINE_STATISTIC_TESSELLATION_EVALUATION_SHADER_INVOCATIONS_BIT;

	queryPoolInfo.queryCount = tools::countBits(queryPoolInfo.pipelineStatistics);
	if (VK_SUCCESS != vkCreateQueryPool(context.device, &queryPoolInfo, NULL, &queryPool))
	{
		// no big deal... just no statisitic avaliable
		//TRACE("Warrning: No pipeline statistics.\n");
	}
	stats.resize(queryPoolInfo.queryCount);
}

PipelineStatistic::~PipelineStatistic()
{
    context.vkDestroy(queryPool);
}

void PipelineStatistic::reset(VkCommandBuffer cmdBuf)
{
    if (queryPool) vkCmdResetQueryPool(cmdBuf, queryPool, 0, static_cast<uint32_t>(stats.size()));
}

void PipelineStatistic::begin(VkCommandBuffer cmdBuf)
{
    if (queryPool) vkCmdBeginQuery(cmdBuf, queryPool, 0, 0);
}

void PipelineStatistic::end(VkCommandBuffer cmdBuf)
{
    if (queryPool) vkCmdEndQuery(cmdBuf, queryPool, 0);
}

void PipelineStatistic::update()
{
    if (queryPool)
    {
        uint32_t count = static_cast<uint32_t>(stats.size());
        vkGetQueryPoolResults(
            context.device,
            queryPool,
            0,
            1,
            count * sizeof(uint64_t),
            stats.data(),
            sizeof(uint64_t),
            VK_QUERY_RESULT_64_BIT);
    }
}
} // namespace renderingEngine