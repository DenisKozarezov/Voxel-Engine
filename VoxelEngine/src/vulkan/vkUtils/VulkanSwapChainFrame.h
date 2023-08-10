#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <vulkan/vkUtils/VulkanAlloc.h>

#define VERTEX_BUFFER_BIND_ID 0
#define INSTANCE_BUFFER_BIND_ID 1

namespace vkUtils
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
		glm::vec3 lightPos;
	};

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

		// Viewport
		VkImage viewportImage;
		VkImageView viewportImageView;
		VkDeviceMemory viewportDeviceMemory;
		VkFramebuffer viewportFramebuffer;
		VkCommandBuffer viewportCommandBuffer;
		VkDescriptorSet viewportDescriptor;
		VkSampler viewportSampler;
		VkDescriptorImageInfo viewportImageInfo;

		void makeDescriptorResources(const VkPhysicalDeviceLimits& limits);

		void writeDescriptorSet() const;

		void release() const;
	};
}