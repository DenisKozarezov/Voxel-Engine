#include "VulkanValidation.h"
#include "../VulkanBackend.h"
#include "../vkInit/VulkanCommand.h"
#include "../vkInit/VulkanInitializers.h"

namespace vkUtils::memory
{
	uint32 findMemoryType(const vkInit::VulkanDevice* device, const uint32& typeFilter, const VkMemoryPropertyFlags& properties)
	{
		for (uint32 i = 0; i < device->memoryProperties.memoryTypeCount; ++i)
		{
			if ((typeFilter & BIT(i)) && (device->memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type!");
	}
	VkDeviceMemory allocateMemory(const vkInit::VulkanDevice* device, const VkMemoryRequirements& requirements, const VkMemoryPropertyFlags& properties)
	{
		const uint32 memoryTypeIndex = findMemoryType(device, requirements.memoryTypeBits, properties);
		const VkMemoryAllocateInfo allocInfo = vkInit::memoryAllocateInfo(requirements.size, memoryTypeIndex);

		VkDeviceMemory memory;
		VkResult err = vkAllocateMemory(device->logicalDevice, &allocInfo, nullptr, &memory);
		VK_CHECK(err, "failed to allocate memory!");

		return memory;
	}
	VkCommandBuffer beginSingleTimeCommands(const VkCommandPool& commandPool)
	{
		const VkCommandBuffer commandBuffer = vkUtils::memory::allocateCommandBuffer(commandPool);
		vkUtils::memory::beginCommand(commandBuffer);
		return commandBuffer;
	}
	
	Buffer createBuffer(const vkInit::VulkanDevice* device, const VkDeviceSize& size, const VkBufferUsageFlags& usage, const VkMemoryPropertyFlags& properties)
	{
		RUNTIME_ASSERT(device->physicalDevice && device->logicalDevice, "failed to create buffer!");

		const VkBufferCreateInfo bufferInfo = vkInit::bufferCreateInfo(usage, size);

		VkBuffer buffer;
		VkResult err = vkCreateBuffer(device->logicalDevice, &bufferInfo, nullptr, &buffer);
		VK_CHECK(err, "failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device->logicalDevice, buffer, &memRequirements);

		const VkDeviceMemory bufferMemory = allocateMemory(device, memRequirements, properties);
		vkBindBufferMemory(device->logicalDevice, buffer, bufferMemory, 0);

		const VkDescriptorBufferInfo descriptorInfo =
		{
			.buffer = buffer,
			.offset = 0,
			.range = size
		};

		Buffer result;
		result.device = device->logicalDevice;
		result.size = size;
		result.buffer = buffer;
		result.bufferMemory = bufferMemory;
		result.descriptor = descriptorInfo;
		return result;
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

	Buffer::Buffer(Buffer&& rhs) noexcept
	{
		this->device = rhs.device;
		std::swap(this->descriptor, rhs.descriptor);
		std::swap(this->size, rhs.size);
		std::swap(this->buffer, rhs.buffer);
		std::swap(this->bufferMemory, rhs.bufferMemory);
		std::swap(this->mappedMemory, rhs.mappedMemory);
	}
	Buffer& Buffer::operator=(Buffer&& rhs) noexcept
	{
		if (this == &rhs)
			return *this;

		release();

		this->device = rhs.device;
		std::swap(this->descriptor, rhs.descriptor);
		std::swap(this->size, rhs.size);
		std::swap(this->buffer, rhs.buffer);
		std::swap(this->bufferMemory, rhs.bufferMemory);
		std::swap(this->mappedMemory, rhs.mappedMemory);

		return *this;
	}
	Buffer::~Buffer()
	{
		release();
	}

	void Buffer::unmap() noexcept
	{
		if (mappedMemory)
		{
			vkUnmapMemory(device, bufferMemory);
			mappedMemory = nullptr;
		}
	}
	void Buffer::release() noexcept
	{
		unmap();

		if (buffer)
		{
			vkDestroyBuffer(device, buffer, nullptr);
			buffer = VK_NULL_HANDLE;
		}

		if (bufferMemory)
		{
			vkFreeMemory(device, bufferMemory, nullptr);
			bufferMemory = VK_NULL_HANDLE;
		}

		descriptor.offset = 0;
		descriptor.range = 0;
		size = 0;
	}
	VkResult Buffer::flush(VkDeviceSize size, VkDeviceSize offset) const
	{
		const VkMappedMemoryRange mappedRange = vkInit::mappedMemoryRange(bufferMemory, offset, size);
		return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
	}
}
