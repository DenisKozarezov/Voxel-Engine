#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources(const VkPhysicalDeviceLimits& limits)
	{
		VkDeviceSize size = sizeof(VoxelEngine::renderer::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			vulkanDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		uniformBuffers.view.map();

		size = sizeof(VoxelEngine::renderer::RaymarchData);
		uniformBuffers.raymarch = vkUtils::memory::createBuffer(
			vulkanDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		uniformBuffers.raymarch.map();
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		/*descriptorWrites[1] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.raymarch.descriptor);*/

		vkUpdateDescriptorSets(vulkanDevice.logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release()
	{
		vkDestroyImageView(vulkanDevice.logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(vulkanDevice.logicalDevice, framebuffer, nullptr);
		vkDestroyFence(vulkanDevice.logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(vulkanDevice.logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(vulkanDevice.logicalDevice, renderFinishedSemaphore, nullptr);

		uniformBuffers.view.release();
		uniformBuffers.raymarch.release();
	}
}