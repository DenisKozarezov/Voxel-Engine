#pragma once
#include "VulkanInitializers.h"
#include "../utils/VulkanValidation.h"

namespace vkInit
{
	struct DescriptorSetLayoutInputBundle
	{
		int count;
		std::vector<uint32> indices;
		std::vector<VkDescriptorType> types;
		std::vector<uint32> counts;
		std::vector<VkShaderStageFlags> stages;
	};

	const VkDescriptorSetLayout createDescriptorSetLayout(const VkDevice& logicalDevice, const DescriptorSetLayoutInputBundle& bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		layoutBindings.reserve(bindings.count);

		for (int i = 0; i < bindings.count; ++i)
		{
			VkDescriptorSetLayoutBinding layoutBinding =
			{
				.binding = bindings.indices[i],
				.descriptorType = bindings.types[i],
				.descriptorCount = bindings.counts[i],
				.stageFlags = bindings.stages[i],
				.pImmutableSamplers = nullptr
			};
			layoutBindings.push_back(layoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = descriptorSetLayoutCreateInfo(layoutBindings);

		VkDescriptorSetLayout layout;
		VkResult err = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &layout);
		vkUtils::check_vk_result(err, "failed to create descriptor set layout!");

		VOXEL_CORE_TRACE("Vulkan descriptor set layout created.")

		return layout;
	}

	const VkDescriptorPool createDescriptorPool(const VkDevice& logicalDevice)
	{
		const auto& pool_sizes = descriptorPoolSize();

		uint32 size = static_cast<uint32>(pool_sizes.size());
		VkDescriptorPoolCreateInfo poolInfo = descriptorPoolCreateInfo(
			pool_sizes.data(),
			size,
			1000 * size,
			VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT);

		VkDescriptorPool pool;
		VkResult err = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &pool);
		vkUtils::check_vk_result(err, "failed to create descriptor pool!");

		VOXEL_CORE_TRACE("Vulkan descriptor pool created.")

		return pool;
	}
}