#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <Core/CoreTypes.h>

namespace vkInit
{
	struct DescriptorSetLayoutInputBundle
	{
		int count = 0;
		std::vector<uint32> indices;
		std::vector<VkDescriptorType> types;
		std::vector<uint32> counts;
		std::vector<VkShaderStageFlags> stages;
	};

	VkDescriptorSetLayout createDescriptorSetLayout(const VkDevice& device, const DescriptorSetLayoutInputBundle& bindings);

	VkDescriptorPool createDescriptorPool(const VkDevice& device);

	VkDescriptorSet allocateDescriptorSet(
		const VkDevice& device,
		const VkDescriptorPool& descriptorPool,
		const VkDescriptorSetLayout& descriptorSetLayout);
}