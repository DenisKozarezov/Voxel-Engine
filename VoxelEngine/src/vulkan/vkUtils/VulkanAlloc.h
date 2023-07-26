#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace vkUtils::memory
{
	struct Buffer
	{
	public:
		VkBuffer buffer;
		VkDeviceMemory bufferMemory;

		void release(const VkDevice& logicalDevice) const
		{
			vkUnmapMemory(logicalDevice, bufferMemory);
			vkDestroyBuffer(logicalDevice, buffer, nullptr);
			vkFreeMemory(logicalDevice, bufferMemory, nullptr);
		}
	};

	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
	
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	
	const VkCommandBuffer& beginSingleTimeCommands();

	void mapMemory(
		const VkDevice& logicalDevice, 
		const VkDeviceMemory& deviceMemory, 
		const VkDeviceSize& offset, 
		const VkDeviceSize& size, 
		const VkMemoryMapFlags& flags, 
		const void* data);

	const Buffer createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	void destroyBuffer(const VkDevice& logicalDevice, const VkBuffer& buffer);
	void freeDeviceMemory(const VkDevice& logicalDevice, const VkDeviceMemory& memory);
}