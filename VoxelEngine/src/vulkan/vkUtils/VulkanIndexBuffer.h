#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanIndexBuffer
	{
	private:
		VkDevice logicalDevice;
		memory::Buffer indexBuffer;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* indices, 
			const size_t& bufferSize);

		operator const VkBuffer&() const & { return indexBuffer.buffer; }

		void release() const;

		~VulkanIndexBuffer() = default;
	};
}