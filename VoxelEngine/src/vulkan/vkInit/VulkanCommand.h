#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace vkInit
{
	const VkCommandPool createCommandPool(const VkDevice& logicalDevice, const uint32& queueFamily);
}

namespace vkUtils::memory
{
	const VkCommandBuffer allocateCommandBuffer(const VkCommandPool& commandPool);
	const std::vector<VkCommandBuffer> allocateCommandBuffer(const VkCommandPool& commandPool, const uint32& buffersCount);

	void resetCommandBuffer(const VkCommandBuffer& buffer);
	void beginCommand(const VkCommandBuffer& buffer);
	void endCommand(const VkCommandBuffer& buffer);
	void releaseCommandBuffer(const VkCommandBuffer& buffer, const VkCommandPool& commandPool);
	void releaseCommandBuffer(const std::vector<VkCommandBuffer>& buffers, const VkCommandPool& commandPool);
}