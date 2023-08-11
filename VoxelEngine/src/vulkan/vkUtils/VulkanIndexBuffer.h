#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanIndexBuffer
	{
	private:
		VkDevice m_logicalDevice;
		memory::Buffer m_indexBuffer;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* indices, 
			const size_t& bufferSize);

		operator const VkBuffer&() const & { return m_indexBuffer.buffer; }

		void release() const;

		~VulkanIndexBuffer() = default;
	};
}