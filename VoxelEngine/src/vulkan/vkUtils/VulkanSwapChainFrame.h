#pragma once
#include <vector>
#include <vulkan/vkUtils/VulkanAlloc.h>
#include <core/renderer/UniformBuffer.h>

namespace vkUtils
{
	typedef struct
	{
		vkUtils::memory::Buffer view;
	} UniformBuffers;

	struct InstanceData
	{
		glm::vec3 pos;
	};

	struct SwapChainFrame
	{
	public:
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;

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

		void release() const;
	};
}