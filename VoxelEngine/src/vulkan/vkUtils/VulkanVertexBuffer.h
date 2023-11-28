#pragma once
#include <core/renderer/Buffer.h>
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		vkInit::VulkanDevice m_device;
		memory::Buffer m_vertexBuffer;
	public:
		VulkanVertexBuffer() noexcept = default;
		VulkanVertexBuffer(const vkInit::VulkanDevice& device, const size_t& bufferSize);
		VulkanVertexBuffer(const vkInit::VulkanDevice& device, const void* vertices, const size_t& bufferSize);
		VulkanVertexBuffer(const VulkanVertexBuffer& rhs);
		VulkanVertexBuffer(VulkanVertexBuffer&& rhs) noexcept;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& rhs);
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&& rhs) noexcept;
		~VulkanVertexBuffer();

		INLINE operator const VkBuffer&() const & { return m_vertexBuffer.buffer; }

		constexpr uint32 size() const override;

		INLINE void setData(const void* data, const uint32& size) override;
		void bind(const uint32& binding = 0) override;
		void bind(const VkCommandBuffer& commandBuffer, const uint32& binding = 0);
		INLINE void release() override;
	};	
}