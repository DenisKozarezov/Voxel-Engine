#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <Core/CoreTypes.h>

namespace vkInit
{
	struct VulkanDevice;
	
	struct DescriptorSetLayoutInputBundle
	{
		int count = 0;
		std::vector<uint32> indices;
		std::vector<VkDescriptorType> types;
		std::vector<uint32> counts;
		std::vector<VkShaderStageFlags> stages;
	};

	VkDescriptorSetLayout createDescriptorSetLayout(const VulkanDevice& device, const DescriptorSetLayoutInputBundle& bindings);

	VkDescriptorPool createDescriptorPool(const VulkanDevice& device);

	VkDescriptorSet allocateDescriptorSet(
		const VulkanDevice& device,
		const VkDescriptorPool& descriptorPool,
		const VkDescriptorSetLayout& descriptorSetLayout);
}