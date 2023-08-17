#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources(const VkPhysicalDeviceLimits& limits)
	{
		VkDeviceSize size = sizeof(VoxelEngine::renderer::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		uniformBuffers.view.map();
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release()
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		uniformBuffers.view.release();
	}
}