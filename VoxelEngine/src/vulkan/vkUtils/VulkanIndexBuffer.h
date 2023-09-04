#pragma once
#include <core/renderer/Buffer.h>
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanIndexBuffer : public VoxelEngine::renderer::IndexBuffer
	{
	private:
		VkPhysicalDevice m_physicalDevice;
		VkDevice m_logicalDevice;
		memory::Buffer m_indexBuffer;
	public:
		VulkanIndexBuffer() noexcept = default;
		VulkanIndexBuffer(
			const VkPhysicalDevice& physicalDevice,
			const VkDevice& logicalDevice,
			const size_t& bufferSize);
		VulkanIndexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* indices, 
			const size_t& bufferSize);
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& buffer);

		INLINE operator const VkBuffer&() const & { return m_indexBuffer.buffer; }

		constexpr uint32 size() const override;
		void bind() override;
		void release();

		~VulkanIndexBuffer() noexcept = default;
	};
}