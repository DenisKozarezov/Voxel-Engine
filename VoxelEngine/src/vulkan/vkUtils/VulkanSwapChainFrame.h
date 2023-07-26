#pragma once
#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include <vulkan/vkUtils/VulkanUniformBuffer.h>

namespace vkUtils
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
	};

	struct SwapChainFrame
	{
		// Swap Chain
		VkImage image;
		VkImageView imageView;
		VkFramebuffer framebuffer;
		VkCommandBuffer commandBuffer;

		// Synchronization
		VkSemaphore imageAvailableSemaphore, renderFinishedSemaphore;
		VkFence inFlightFence;
		
		// Resources used in drawing
		vkUtils::VulkanUniformBuffer uniformBuffer;
		vkUtils::memory::Buffer modelBuffer;
		void* modelBufferMappedMemory;
		VkDescriptorSet descriptorSet;
		VkDescriptorBufferInfo uniformBufferDescriptor;
		VkDescriptorBufferInfo modelBufferDescriptor;
		std::vector<glm::mat4> modelTransforms;

		void makeDescriptorResources(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice)
		{
			// UNIFORM BUFFER
			VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
			uniformBuffer = vkUtils::VulkanUniformBuffer(physicalDevice, logicalDevice, size);

			uniformBufferDescriptor.buffer = uniformBuffer;
			uniformBufferDescriptor.offset = 0;
			uniformBufferDescriptor.range = size;

			// STORAGE BUFFER
			size = 1024 * sizeof(glm::mat4);
			modelBuffer = vkUtils::memory::createBuffer(
				physicalDevice,
				logicalDevice,
				size,
				VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

			vkMapMemory(logicalDevice, modelBuffer.bufferMemory, 0, size, 0, &modelBufferMappedMemory);

			modelTransforms.reserve(1024);
			for (int i = 0; i < 1024; ++i)
			{
				modelTransforms.push_back(glm::mat4(1.0f));
			}

			modelBufferDescriptor.buffer = modelBuffer.buffer;
			modelBufferDescriptor.offset = 0;
			modelBufferDescriptor.range = size;
		}

		void writeDescriptorSet(const VkDevice& logicalDevice)
		{
			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSet;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0; // byte offset within binding for inline uniform blocks
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].pBufferInfo = &uniformBufferDescriptor;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSet;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0; // byte offset within binding for inline uniform blocks
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].pBufferInfo = &modelBufferDescriptor;

			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	};
}