#include "VulkanCommand.h"
#include "VulkanInitializers.h"
#include "../vkUtils/VulkanValidation.h" 
#include "../VulkanBackend.h"

namespace vkInit
{
	VkCommandPool createCommandPool(const VkDevice& device, const uint32& queueFamily)
	{
		const VkCommandPoolCreateInfo poolInfo = commandPoolCreateInfo(queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		VkCommandPool commandPool;
		VkResult err = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
		VK_CHECK(err, "failed to create command pool!");

		RUNTIME_TRACE("Vulkan command pool created.")

		return commandPool;
	}
}

namespace vkUtils::memory
{
	VkCommandBuffer allocateCommandBuffer(const VkCommandPool& commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		const auto* device = vulkan::getDevice();
		vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, &commandBuffer);

		return commandBuffer;
	}
	std::vector<VkCommandBuffer> allocateCommandBuffer(const VkCommandPool& commandPool, const uint32& buffersCount)
	{
		std::vector<VkCommandBuffer> buffers(buffersCount);

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = buffersCount;

		const auto* device = vulkan::getDevice();
		VkResult err = vkAllocateCommandBuffers(device->logicalDevice, &allocInfo, buffers.data());
		VK_CHECK(err, "failed to allocate command buffers!");

		return buffers;
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
}