#pragma once
#include <vulkan/vulkan.h>
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

namespace vkUtils::memory
{
	struct Buffer
	{
	public:
		VkDevice logicalDevice = VK_NULL_HANDLE;
		VkDeviceSize size = 0;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		void* mappedMemory = nullptr;
		VkDescriptorBufferInfo descriptor;

		INLINE operator const VkBuffer& () const& { return buffer; }

		INLINE void map()
		{
			vkMapMemory(logicalDevice, bufferMemory, 0, size, 0, &mappedMemory);
		}
		INLINE void setData(const void* data, const size_t& size) const 
		{ 
			memcpy(mappedMemory, data, size); 
		}
		void unmap();
		void release();
		VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0) const;
	};

	const uint32 findMemoryType(const VkPhysicalDevice& physicalDevice, const uint32& typeFilter, const VkMemoryPropertyFlags& properties);
	
	const VkDeviceMemory allocateMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	
	const VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool);

	Buffer createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	constexpr uint32 alignedSize(uint32 value, uint32 alignment);
	constexpr size_t alignedSize(size_t value, size_t alignment);

	void* alignedAlloc(size_t size, size_t alignment);
	void alignedFree(void* data);
}