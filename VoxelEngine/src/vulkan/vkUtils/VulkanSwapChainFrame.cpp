#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

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

		size = sizeof(glm::mat4) * 1024;
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

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBufferDescriptor);

		descriptorWrites[1] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			1,
			&modelBufferDescriptor);

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
	}
}