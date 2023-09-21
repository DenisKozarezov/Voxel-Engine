#pragma once
#include <core/renderer/Buffer.h>
#include <vulkan/vkUtils/VulkanAlloc.h>
#include <vulkan/vkInit/VulkanInitializers.h>

namespace vkUtils
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		VkDevice m_logicalDevice = VK_NULL_HANDLE;
		VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
		memory::Buffer m_vertexBuffer;
	public:
		VulkanVertexBuffer() noexcept = default;
		VulkanVertexBuffer(
			const VkPhysicalDevice& physicalDevice,
			const VkDevice& logicalDevice,
			const size_t& bufferSize);
		VulkanVertexBuffer(
			const VkPhysicalDevice& physicalDevice, 
			const VkDevice& logicalDevice, 
			const void* vertices,
			const size_t& bufferSize);
		VulkanVertexBuffer(const VulkanVertexBuffer& rhs);
		VulkanVertexBuffer(VulkanVertexBuffer&& rhs);
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& buffer);

		INLINE operator const VkBuffer&() const & { return m_vertexBuffer.buffer; }

		constexpr uint32 size() const override;

		void setData(const void* data, const uint32& size) override;
		INLINE void bind() override { bind(VERTEX_BUFFER_BIND_ID); }
		void bind(const uint32& binding);
		void bind(const VkCommandBuffer& commandBuffer, const uint32& binding);
		void release() override;

		~VulkanVertexBuffer();
	};	
}