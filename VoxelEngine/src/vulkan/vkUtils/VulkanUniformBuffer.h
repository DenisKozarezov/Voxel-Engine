#pragma once
#include "VulkanAlloc.h"

namespace vkUtils
{
	class VulkanUniformBuffer
	{
	private:
		VkDevice m_logicalDevice;
		memory::Buffer m_uniformBuffer;
		void* m_uniformBufferMapped;
	public:
		VulkanUniformBuffer() = default;
		VulkanUniformBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const uint32& size);

		operator const VkBuffer&() const & { return m_uniformBuffer.buffer; }

		void setData(const void* data, size_t size) const;
		void release() const;

		~VulkanUniformBuffer() = default;
	};
}