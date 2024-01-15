#pragma once
#include <vector>
#include <core/Base.h>
#include <core/PrimitiveTypes.h>
#include <vulkan/vulkan.h>

namespace vkInit
{
	struct VulkanDevice;
}

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
	INLINE std::vector<uint64> pipelineStats;

	VkQueryPool createQueryPool(const VkDevice& logicalDevice);

	void getQueryResults(const vkInit::VulkanDevice& device);
}
