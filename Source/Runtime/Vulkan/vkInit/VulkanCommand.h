#pragma once
#include <vector>
#include <Core/CoreDefines.h>
#include <Core/CoreTypes.h>
#include <vulkan/vulkan.h>

namespace vkInit
{
	VkCommandPool createCommandPool(const VkDevice& device, const uint32& queueFamily);
}

namespace vkUtils::memory
{
	VkCommandBuffer allocateCommandBuffer(const VkCommandPool& commandPool);
	std::vector<VkCommandBuffer> allocateCommandBuffer(const VkCommandPool& commandPool, const uint32& buffersCount);

	INLINE void resetCommandBuffer(const VkCommandBuffer& buffer)
	{
		vkResetCommandBuffer(buffer, /*VkCommandBufferResetFlagBits*/ 0);
	}
	void beginCommand(const VkCommandBuffer& buffer);
	void endCommand(const VkCommandBuffer& buffer);
	INLINE void releaseCommandBuffer(const VkDevice& device, const VkCommandBuffer& buffer, const VkCommandPool& commandPool)
	{
		vkFreeCommandBuffers(device, commandPool, 1, &buffer);
	}
	INLINE void releaseCommandBuffer(const VkDevice& device, const std::vector<VkCommandBuffer>& buffers, const VkCommandPool& commandPool)
	{
		vkFreeCommandBuffers(device, commandPool, static_cast<uint32>(buffers.size()), buffers.data());
	}
}