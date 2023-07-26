#include "VulkanCommandBuffer.h"
#include "VulkanRenderer.h"

namespace VoxelEngine::renderer
{
	VkCommandBuffer CommandBuffer::allocate()
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = VulkanRenderer::getInstance()->getCommandPool();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		auto logicalDevice = VulkanRenderer::getInstance()->getLogicalDevice();
		vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

		return commandBuffer;
	}
	std::vector<VkCommandBuffer> CommandBuffer::allocate(const uint32& buffersCount)
	{
		std::vector<VkCommandBuffer> buffers(buffersCount);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = VulkanRenderer::getInstance()->getCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = buffersCount;

		auto logicalDevice = VulkanRenderer::getInstance()->getLogicalDevice();
		VkResult err = vkAllocateCommandBuffers(logicalDevice, &allocInfo, buffers.data());
		check_vk_result(err, "failed to allocate command buffers!");

		return buffers;
	}
	void CommandBuffer::reset(const VkCommandBuffer& buffer)
	{
		vkResetCommandBuffer(buffer, /*VkCommandBufferResetFlagBits*/ 0);
	}
	void CommandBuffer::beginCommand(const VkCommandBuffer& buffer)
	{
		VkCommandBufferBeginInfo begin_info = {};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VkResult err = vkBeginCommandBuffer(buffer, &begin_info);
		check_vk_result(err, "failed to begin recording command buffer!");
	}
	void CommandBuffer::endCommand(const VkCommandBuffer& buffer)
	{
		VkResult err = vkEndCommandBuffer(buffer);
		check_vk_result(err, "failed to record command buffer!");
	}
	void CommandBuffer::release(const VkCommandBuffer& buffer)
	{
		auto logicalDevice = VulkanRenderer::getInstance()->getLogicalDevice();
		auto commandPool = VulkanRenderer::getInstance()->getCommandPool();
		vkFreeCommandBuffers(logicalDevice, commandPool, 1, &buffer);
	}
	void CommandBuffer::release(const std::vector<VkCommandBuffer>& buffers)
	{
		auto logicalDevice = VulkanRenderer::getInstance()->getLogicalDevice();
		auto commandPool = VulkanRenderer::getInstance()->getCommandPool();
		vkFreeCommandBuffers(logicalDevice, commandPool, static_cast<uint32>(buffers.size()), buffers.data());
	}
}