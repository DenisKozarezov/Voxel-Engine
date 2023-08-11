#include "VulkanAlloc.h"
#include "VulkanValidation.h"
#include "../VulkanBackend.h"
#include "../vkInit/VulkanCommand.h"

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
	const VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool)
	{
		VkCommandBuffer commandBuffer = vkUtils::memory::allocateCommandBuffer(commandPool);
		vkUtils::memory::beginCommand(commandBuffer);
		return commandBuffer;
	}
	
	Buffer createBuffer(const VkPhysicalDevice& physicalDevice, const VkDevice& logicalDevice, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties)
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

		VkDescriptorBufferInfo descriptorInfo =
		{
			.buffer = buffer,
			.offset = 0,
			.range = size
		};

		Buffer result =
		{
			.logicalDevice = logicalDevice,
			.size = size,
			.buffer = buffer,
			.bufferMemory = bufferMemory,
			.descriptor = descriptorInfo
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

	constexpr uint32 alignedSize(uint32 value, uint32 alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}
	constexpr size_t alignedSize(size_t value, size_t alignment)
	{
		return (value + alignment - 1) & ~(alignment - 1);
	}

	void* alignedAlloc(size_t size, size_t alignment)
	{
		void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
		data = _aligned_malloc(size, alignment);
#else
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
#endif
		return data;
	}
	void alignedFree(void* data)
	{
#if	defined(_MSC_VER) || defined(__MINGW32__)
		_aligned_free(data);
#else
		free(data);
#endif
	}
}