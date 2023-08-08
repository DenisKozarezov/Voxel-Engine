#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>

namespace vkUtils::memory
{
	struct Buffer
	{
	public:
		VkDevice logicalDevice;
		VkDeviceSize size = 0;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		void* mappedMemory = nullptr;
		VkDescriptorBufferInfo descriptor;

		void release() const
		{			
			if (buffer)
				vkDestroyBuffer(logicalDevice, buffer, nullptr);

			if (bufferMemory)
				vkFreeMemory(logicalDevice, bufferMemory, nullptr);
		}
		VkResult map()
		{
			return vkMapMemory(logicalDevice, bufferMemory, 0, size, 0, &mappedMemory);
		}
		void unmap()
		{
			if (mappedMemory)
			{
				vkUnmapMemory(logicalDevice, bufferMemory);
				mappedMemory = nullptr;
			}
		}
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = bufferMemory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkFlushMappedMemoryRanges(logicalDevice, 1, &mappedRange);
		}
	};

	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
	
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	
	const VkCommandBuffer& beginSingleTimeCommands(const VkCommandPool& commandPool);

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

	constexpr uint32 alignedSize(uint32 value, uint32 alignment);
	constexpr size_t alignedSize(size_t value, size_t alignment);

	void* alignedAlloc(size_t size, size_t alignment);
	void alignedFree(void* data);
}