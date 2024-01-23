#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>
#include <Vulkan/vkInit/VulkanDevice.h>

namespace vkUtils
{
	typedef struct
	{
		memory::Buffer view;
	} UniformBuffers;

	struct SwapChainFrame
	{
	public:
		const vkInit::VulkanDevice* vulkanDevice = nullptr;

		VkImage image;
		VkImageView imageView;
		VkFramebuffer framebuffer;
		VkCommandBuffer commandBuffer;

		// Synchronization
		VkSemaphore imageAvailableSemaphore, renderFinishedSemaphore;
		VkFence inFlightFence;
		
		// Resources used in drawing
		UniformBuffers uniformBuffers;
		VkDescriptorSet descriptorSet;

		void makeDescriptorResources(const VkPhysicalDeviceLimits& limits);

		void writeDescriptorSet() const;

		void release();
	};
}