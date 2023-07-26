#pragma once
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vulkan
{
	class VulkanIndexBuffer
	{
	private:
		VkDevice logicalDevice;
		vkUtils::memory::Buffer indexBuffer;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(const void* indices, const size_t& bufferSize);

		operator const VkBuffer&() const & { return indexBuffer.buffer; }

		void bind() const;
		void release() const;
		void setData(const void* data, const size_t& size);

		~VulkanIndexBuffer() = default;
	};
}