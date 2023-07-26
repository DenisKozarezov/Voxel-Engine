#pragma once
#include "VulkanAlloc.h"

namespace vkUtils
{
	class VulkanUniformBuffer
	{
	private:
		VkDevice logicalDevice;
		memory::Buffer uniformBuffer;
		void* uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const uint32& size);

		operator const VkBuffer&() const & { return uniformBuffer.buffer; }

		void setData(const void* data, size_t size) const;
		void release() const;

		~VulkanUniformBuffer() = default;
	};
}