#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources()
	{
		// UNIFORM BUFFER
		VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		size = sizeof(glm::mat4) * 1024;
		uniformBuffers.dynamic = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
		
		uniformBuffers.view.map(logicalDevice);
		uniformBuffers.dynamic.map(logicalDevice);

		modelTransforms.reserve(1024);
		for (int i = 0; i < 1024; ++i)
		{
			modelTransforms.push_back(glm::mat4(1.0f));
		}
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		descriptorWrites[1] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
			1,
			&uniformBuffers.dynamic.descriptor);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		uniformBuffers.view.release(logicalDevice);
		uniformBuffers.dynamic.release(logicalDevice);
	}
}