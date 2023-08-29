#pragma once
#include <vector>
#include "VulkanValidation.h"

namespace vkUtils
{
	const std::vector<string> pipelineStatNames =
	{
		"Input assembly vertex count        ",
		"Input assembly primitives count    ",
		"Vertex shader invocations          ",
		"Clipping stage primitives processed",
		"Clipping stage primitives output   ",
		"Fragment shader invocations        "
	};
	std::vector<uint64> pipelineStats;

	const VkQueryPool setupQueryPool(const VkDevice& logicalDevice)
	{
		VkQueryPoolCreateInfo queryPoolInfo{};
		queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
		queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;

		// Pipeline counters to be returned for this pool
		queryPoolInfo.pipelineStatistics =
			VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_VERTICES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_INPUT_ASSEMBLY_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_VERTEX_SHADER_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT |
			VK_QUERY_PIPELINE_STATISTIC_CLIPPING_PRIMITIVES_BIT |
			VK_QUERY_PIPELINE_STATISTIC_FRAGMENT_SHADER_INVOCATIONS_BIT;

		queryPoolInfo.queryCount = static_cast<uint32>(pipelineStatNames.size());

		pipelineStats.reserve(queryPoolInfo.queryCount);

		VkQueryPool queryPool;
		VkResult err = vkCreateQueryPool(logicalDevice, &queryPoolInfo, NULL, &queryPool);
		VK_CHECK(err, "failed to create statistics query pool!");
		return queryPool;
	}

	void getQueryResults(const VkDevice& logicalDevice, const VkQueryPool& queryPool)
	{
		uint32 count = static_cast<uint32>(pipelineStats.size());
		vkGetQueryPoolResults(
			logicalDevice,
			queryPool,
			0,
			1,
			count * sizeof(uint64),
			pipelineStats.data(),
			sizeof(uint64),
			VK_QUERY_RESULT_64_BIT);
	}
}