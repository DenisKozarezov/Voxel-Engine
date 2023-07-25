#pragma once
#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include <vulkan/vkUtils/VulkanUniformBuffer.h>

namespace vkUtils
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
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
		vkUtils::VulkanUniformBuffer* uniformBuffer;
		VkDescriptorSet descriptorSet;
		VkDescriptorBufferInfo uniformBufferDescriptor;

		void writeDescriptorSet(const VkDevice& logicalDevice)
		{
			VkWriteDescriptorSet writeInfo{};
			writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeInfo.dstSet = descriptorSet;
			writeInfo.dstBinding = 0;
			writeInfo.dstArrayElement = 0; // byte offset within binding for inline uniform blocks
			writeInfo.descriptorCount = 1;
			writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeInfo.pBufferInfo = &uniformBufferDescriptor;

			vkUpdateDescriptorSets(logicalDevice, 1, &writeInfo, 0, nullptr);
		}
	};
}