#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <vulkan/vkUtils/VulkanUniformBuffer.h>

#define VERTEX_BUFFER_BIND_ID 0
#define INSTANCE_BUFFER_BIND_ID 1
#define INSTANCES_COUNT 100 * 100 * 100

namespace vkUtils
{
	struct UniformBufferObject
	{
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 viewproj;
	};

	typedef struct
	{
		vkUtils::memory::Buffer view;
		vkUtils::memory::Buffer instances;
	} UniformBuffers;

	struct InstanceData
	{
		glm::vec3 pos;
		float scale;
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