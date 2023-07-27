#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <vulkan/vkUtils/VulkanUniformBuffer.h>

namespace vkUtils
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
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
		vkUtils::VulkanUniformBuffer uniformBuffer;
		vkUtils::memory::Buffer modelBuffer;
		void* modelBufferMappedMemory;
		VkDescriptorSet descriptorSet;
		VkDescriptorBufferInfo uniformBufferDescriptor;
		VkDescriptorBufferInfo modelBufferDescriptor;
		std::vector<glm::mat4> modelTransforms;

		void makeDescriptorResources();

		void writeDescriptorSet() const;

		void release() const;
	};
}