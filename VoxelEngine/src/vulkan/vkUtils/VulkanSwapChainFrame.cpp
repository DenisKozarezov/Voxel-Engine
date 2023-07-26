#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkUtils/VulkanImage.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources()
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

	void SwapChainFrame::makeDepthResources()
	{
		depthFormat = vkUtils::findDepthFormat(physicalDevice);

		depthImage = vkUtils::createImage(
			physicalDevice,
			logicalDevice, 
			width, height,
			depthFormat,
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			depthImageMemory);

		depthImageView = vkUtils::createImageView(
			logicalDevice,
			depthImage,
			depthFormat,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void SwapChainFrame::writeDescriptorSet() const
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

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		uniformBuffer.release();
		modelBuffer.release(logicalDevice);

		vkDestroyImage(logicalDevice, depthImage, nullptr);
		vkDestroyImageView(logicalDevice, depthImageView, nullptr);
		vkFreeMemory(logicalDevice, depthImageMemory, nullptr);
	}
}