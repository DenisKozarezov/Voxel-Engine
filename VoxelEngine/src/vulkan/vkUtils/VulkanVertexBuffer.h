#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanVertexBuffer
	{
	private:
		VkDevice m_logicalDevice;
		vkUtils::memory::Buffer m_vertexBuffer;
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* vertices, 
			const size_t& bufferSize);

		operator const VkBuffer&() const & { return m_vertexBuffer.buffer; }

		void release() const;

		~VulkanVertexBuffer();
	};	
}