#include "VulkanSwapChainFrame.h"
#include <array>
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils
{
	void SwapChainFrame::makeDescriptorResources(const VkPhysicalDeviceLimits& limits)
	{
		VkDeviceSize size = sizeof(vkUtils::UniformBufferObject);
		uniformBuffers.view = vkUtils::memory::createBuffer(
			physicalDevice,
			logicalDevice,
			size,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

		uniformBuffers.view.map();

		//viewportImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//viewportImageInfo.imageView = viewportImageView;
		//viewportImageInfo.sampler = viewportSampler;
	}

	void SwapChainFrame::writeDescriptorSet() const
	{
		std::array<VkWriteDescriptorSet, 1> descriptorWrites = {};
		descriptorWrites[0] = vkInit::writeDescriptorSet(
			descriptorSet,
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			0,
			&uniformBuffers.view.descriptor);

		//descriptorWrites[1] = vkInit::writeDescriptorSet(
		//	descriptorSet,
		//	VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		//	1,
		//	&viewportImageInfo);

		vkUpdateDescriptorSets(logicalDevice, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void SwapChainFrame::release() const
	{
		vkDestroyImageView(logicalDevice, imageView, nullptr);
		vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		vkDestroyFence(logicalDevice, inFlightFence, nullptr);
		vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
		vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);

		//vkDestroyImageView(logicalDevice, viewportImageView, nullptr);
		//vkDestroyFramebuffer(logicalDevice, viewportFramebuffer, nullptr);
		//vkFreeMemory(logicalDevice, viewportDeviceMemory, nullptr);

		uniformBuffers.view.release();
	}
}