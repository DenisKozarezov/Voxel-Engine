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
		VulkanIndexBuffer(const VulkanIndexBuffer& rhs);
		VulkanIndexBuffer(VulkanIndexBuffer&& rhs) noexcept;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& rhs);
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&& rhs);
		~VulkanIndexBuffer();

		INLINE operator const VkBuffer&() const & { return m_indexBuffer.buffer; }

		constexpr uint32 size() const override;
		void bind() override;
		INLINE void release()
		{
			m_indexBuffer.release();
		}
	};
}