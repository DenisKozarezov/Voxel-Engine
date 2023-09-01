#pragma once
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h"

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

	const VkDescriptorSetLayout createDescriptorSetLayout(const VkDevice& logicalDevice, const DescriptorSetLayoutInputBundle& bindings)
	{
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		layoutBindings.reserve(bindings.count);

		for (int i = 0; i < bindings.count; ++i)
		{
			VkDescriptorSetLayoutBinding layoutBinding = vkInit::descriptorSetLayoutBinding(
				bindings.types[i],
				bindings.stages[i],
				bindings.indices[i],
				bindings.counts[i]
			);
			layoutBindings.push_back(layoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = descriptorSetLayoutCreateInfo(layoutBindings);

		VkDescriptorSetLayout layout;
		VkResult err = vkCreateDescriptorSetLayout(logicalDevice, &layoutInfo, nullptr, &layout);
		VK_CHECK(err, "failed to create descriptor set layout!");

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
			1000 * size);

		VkDescriptorPool pool;
		VkResult err = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &pool);
		VK_CHECK(err, "failed to create descriptor pool!");

		VOXEL_CORE_TRACE("Vulkan descriptor pool created.")

		return pool;
	}

	const VkDescriptorSet allocateDescriptorSet(
		const VkDevice& logicalDevice,
		const VkDescriptorPool& descriptorPool,
		const VkDescriptorSetLayout& descriptorSetLayout)
	{
		VkDescriptorSetAllocateInfo allocInfo = descriptorSetAllocateInfo(
			descriptorPool,
			&descriptorSetLayout
		);

		VkDescriptorSet descriptorSet;
		VkResult err = vkAllocateDescriptorSets(logicalDevice, &allocInfo, &descriptorSet);
		VK_CHECK(err, "failed to allocate descriptor set!");

		VOXEL_CORE_TRACE("Vulkan descriptor set allocated.")

		return descriptorSet;
	}
}