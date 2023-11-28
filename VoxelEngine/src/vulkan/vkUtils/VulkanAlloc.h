#pragma once
#include <vulkan/vkInit/VulkanDevice.h>

namespace vkUtils::memory
{
	struct Buffer
	{
	public:
		vkInit::VulkanDevice device;
		VkDeviceSize size = 0;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
		void* mappedMemory = nullptr;
		VkDescriptorBufferInfo descriptor{};

		Buffer() noexcept = default;
		Buffer(const Buffer&) = default;
		Buffer(Buffer&& rhs) noexcept;
		Buffer& operator=(const Buffer& rhs) = default;
		Buffer& operator=(Buffer&& rhs);
		~Buffer();

		INLINE operator const VkBuffer& () const& { return buffer; }

		INLINE void map()
		{
			vkMapMemory(device.logicalDevice, bufferMemory, 0, size, 0, &mappedMemory);
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
	
	const VkDeviceMemory allocateMemory(const vkInit::VulkanDevice& device, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties);
	
	const VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool);

	Buffer createBuffer(const vkInit::VulkanDevice& device, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	constexpr uint32 alignedSize(uint32 value, uint32 alignment);
	constexpr size_t alignedSize(size_t value, size_t alignment);

	void* alignedAlloc(size_t size, size_t alignment);
	void alignedFree(void* data);
}