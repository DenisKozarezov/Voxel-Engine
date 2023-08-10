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
			1000 * size);

		VkDescriptorPool pool;
		VkResult err = vkCreateDescriptorPool(logicalDevice, &poolInfo, nullptr, &pool);
		vkUtils::check_vk_result(err, "failed to create descriptor pool!");

		VOXEL_CORE_TRACE("Vulkan descriptor pool created.")

		return pool;
	}

	const VkDescriptorSet allocateDescriptorSet(
		const VkDevice& logicalDevice,
		const VkDescriptorPool& descriptorPool,
		const VkDescriptorSetLayout& descriptorSetLayout)
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &descriptorSetLayout;

		VkDescriptorSet descriptorSet;
		VkResult err = vkAllocateDescriptorSets(logicalDevice, &allocInfo, &descriptorSet);
		vkUtils::check_vk_result(err, "failed to allocate descriptor set!");

		VOXEL_CORE_TRACE("Vulkan descriptor set allocated.")

		return descriptorSet;
	}
}