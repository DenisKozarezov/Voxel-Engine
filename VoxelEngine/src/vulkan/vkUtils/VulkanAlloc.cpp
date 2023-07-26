#include "VulkanAlloc.h"
#include "VulkanValidation.h"
#include "VulkanCommandBuffer.h"

namespace vkUtils::memory
{
	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32 i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		VkDeviceMemory memory;

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = requirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, requirements.memoryTypeBits, properties);

		VkResult err = vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &memory);
		vkUtils::check_vk_result(err, "failed to allocate memory!");

		return memory;
	}
	const VkCommandBuffer& beginSingleTimeCommands()
	{
		VkCommandBuffer commandBuffer = CommandBuffer::allocate();
		CommandBuffer::beginCommand(commandBuffer);
		return commandBuffer;
	}
	
	void mapMemory(
		const VkDevice& logicalDevice,
		const VkDeviceMemory& deviceMemory,
		const VkDeviceSize& offset,
		const VkDeviceSize& size,
		const VkMemoryMapFlags& flags,
		const void* data)
	{
		void* dst;
		vkMapMemory(logicalDevice, deviceMemory, offset, size, flags, &dst);
		memcpy(dst, data, size);
		vkUnmapMemory(logicalDevice, deviceMemory);
	}
	
	const Buffer createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties)
	{
		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkBuffer buffer;
		VkResult err = vkCreateBuffer(logicalDevice, &bufferInfo, nullptr, &buffer);
		vkUtils::check_vk_result(err, "failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(logicalDevice, buffer, &memRequirements);

		VkDeviceMemory bufferMemory = allocateMemory(physicalDevice, logicalDevice, memRequirements, properties);

		vkBindBufferMemory(logicalDevice, buffer, bufferMemory, 0);

		Buffer result =
		{
			.buffer = buffer,
			.bufferMemory = bufferMemory
		};
		return result;
	}
	void destroyBuffer(const VkDevice& logicalDevice, const VkBuffer& buffer)
	{
		vkDestroyBuffer(logicalDevice, buffer, nullptr);
	}
	void freeDeviceMemory(const VkDevice& logicalDevice, const VkDeviceMemory& memory)
	{
		vkFreeMemory(logicalDevice, memory, nullptr);
	}
}