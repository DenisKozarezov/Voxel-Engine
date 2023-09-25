#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

namespace vkInit
{
	const VkCommandPool createCommandPool(const VkDevice& logicalDevice, const uint32& queueFamily);
}

namespace vkUtils::memory
{
	const VkCommandBuffer allocateCommandBuffer(const VkCommandPool& commandPool);
	const std::vector<VkCommandBuffer> allocateCommandBuffer(const VkCommandPool& commandPool, const uint32& buffersCount);

	INLINE void resetCommandBuffer(const VkCommandBuffer& buffer)
	{
		vkResetCommandBuffer(buffer, /*VkCommandBufferResetFlagBits*/ 0);
	}
	void beginCommand(const VkCommandBuffer& buffer);
	void endCommand(const VkCommandBuffer& buffer);
	INLINE void releaseCommandBuffer(const VkDevice& logicalDevice, const VkCommandBuffer& buffer, const VkCommandPool& commandPool)
	{
		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &buffer);
	}
	INLINE void releaseCommandBuffer(const VkDevice& logicalDevice, const std::vector<VkCommandBuffer>& buffers, const VkCommandPool& commandPool)
	{
		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32>(buffers.size()), buffers.data());
	}
}