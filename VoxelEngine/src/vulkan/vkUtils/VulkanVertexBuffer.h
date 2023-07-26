#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanVertexBuffer
	{
	private:
		VkDevice logicalDevice;
		vkUtils::memory::Buffer vertexBuffer;
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* vertices, 
			const size_t& bufferSize);

		operator const VkBuffer&() const & { return vertexBuffer.buffer; }

		void release() const;

		~VulkanVertexBuffer();
	};	
}