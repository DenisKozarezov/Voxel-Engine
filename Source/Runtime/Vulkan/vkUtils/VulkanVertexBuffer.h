#pragma once
#include <Renderer/Buffer.h>
#include <Vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		const vkInit::VulkanDevice* m_device = nullptr;
		memory::Buffer m_vertexBuffer;

		FORCE_INLINE void release() override { m_vertexBuffer.release(); }
	public:
		VulkanVertexBuffer() noexcept = default;
		VulkanVertexBuffer(const vkInit::VulkanDevice* device, const size_t& bufferSize);
		VulkanVertexBuffer(const vkInit::VulkanDevice* device, const void* vertices, const size_t& bufferSize);
		VulkanVertexBuffer(const VulkanVertexBuffer& rhs);
		VulkanVertexBuffer(VulkanVertexBuffer&& rhs) noexcept;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& rhs);
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&& rhs) noexcept;
		~VulkanVertexBuffer() override;

		FORCE_INLINE operator const VkBuffer&() const & { return m_vertexBuffer.buffer; }

		[[nodiscard]] constexpr uint32 size() const override;

		INLINE void setData(const void* data, const uint32& size) override;
		void bind(const uint32& binding = 0) override;
		void bind(const VkCommandBuffer& commandBuffer, const uint32& binding = 0);
	};	
}