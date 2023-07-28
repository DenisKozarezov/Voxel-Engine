#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources()
	{
		// UNIFORM BUFFER
		VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
		VSuniformBuffer = vkUtils::VulkanUniformBuffer(physicalDevice, logicalDevice, size);

		VSuniformBufferDescriptor.buffer = VSuniformBuffer;
		VSuniformBufferDescriptor.offset = 0;
		VSuniformBufferDescriptor.range = size;
		
		GSuniformBuffer = vkUtils::VulkanUniformBuffer(physicalDevice, logicalDevice, size);

		GSuniformBufferDescriptor.buffer = GSuniformBuffer;
		GSuniformBufferDescriptor.offset = 0;
		GSuniformBufferDescriptor.range = size;
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&VSuniformBufferDescriptor);

		descriptorWrites[1] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1,
			&GSuniformBufferDescriptor);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		VSuniformBuffer.release();
		GSuniformBuffer.release();
	}
}