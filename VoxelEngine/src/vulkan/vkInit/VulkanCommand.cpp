#include "VulkanCommand.h"
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h" 
#include "../VulkanBackend.h"

namespace vkInit
{
	const VkCommandPool createCommandPool(
		const VkDevice& logicalDevice,
		const uint32& queueFamily)
	{
		VkCommandPoolCreateInfo poolInfo = vkInit::commandPoolCreateInfo(queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		VkCommandPool commandPool;
		VkResult err = vkCreateCommandPool(logicalDevice, &poolInfo, nullptr, &commandPool);
		VK_CHECK(err, "failed to create command pool!");

		VOXEL_CORE_TRACE("Vulkan command pool created.")

		return commandPool;
	}
}

namespace vkUtils::memory
{
	const VkCommandBuffer allocateCommandBuffer(const VkCommandPool& commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		auto logicalDevice = vulkan::getLogicalDevice();
		vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

		return commandBuffer;
	}
	const std::vector<VkCommandBuffer> allocateCommandBuffer(const VkCommandPool& commandPool, const uint32& buffersCount)
	{
		std::vector<VkCommandBuffer> buffers(buffersCount);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = buffersCount;

		auto logicalDevice = vulkan::getLogicalDevice();
		VkResult err = vkAllocateCommandBuffers(logicalDevice, &allocInfo, buffers.data());
		VK_CHECK(err, "failed to allocate command buffers!");

		return buffers;
	}
	void resetCommandBuffer(const VkCommandBuffer& buffer)
	{
		vkResetCommandBuffer(buffer, /*VkCommandBufferResetFlagBits*/ 0);
	}
	void beginCommand(const VkCommandBuffer& buffer)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkResult err = vkBeginCommandBuffer(buffer, &begin_info);
		VK_CHECK(err, "failed to begin recording command buffer!");
	}
	void endCommand(const VkCommandBuffer& buffer)
	{
		VkResult err = vkEndCommandBuffer(buffer);
		VK_CHECK(err, "failed to record command buffer!");
	}
	void releaseCommandBuffer(const VkCommandBuffer& buffer, const VkCommandPool& commandPool)
	{
		auto logicalDevice = vulkan::getLogicalDevice();
		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &buffer);
	}
	void releaseCommandBuffer(const std::vector<VkCommandBuffer>& buffers, const VkCommandPool& commandPool)
	{
		auto logicalDevice = vulkan::getLogicalDevice();
		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32>(buffers.size()), buffers.data());
	}
}