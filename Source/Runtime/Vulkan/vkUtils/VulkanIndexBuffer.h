#pragma once
#include <Renderer/Buffer.h>
#include <vulkan/vkUtils/VulkanAlloc.h>

namespace vkUtils
{
	class VulkanIndexBuffer : public VoxelEngine::renderer::IndexBuffer
	{
	private:
		const vkInit::VulkanDevice* m_device = nullptr;
		memory::Buffer m_indexBuffer;

		FORCE_INLINE void release() override { m_indexBuffer.release(); }
	public:
		VulkanIndexBuffer() noexcept = default;
		VulkanIndexBuffer(const vkInit::VulkanDevice*  device, const size_t& bufferSize);
		VulkanIndexBuffer(const vkInit::VulkanDevice*  device, const void* indices, const size_t& bufferSize);
		VulkanIndexBuffer(const VulkanIndexBuffer& rhs);
		VulkanIndexBuffer(VulkanIndexBuffer&& rhs) noexcept;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& rhs);
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&& rhs);
		~VulkanIndexBuffer() override;

		FORCE_INLINE operator const VkBuffer&() const & { return m_indexBuffer.buffer; }

		constexpr uint32 size() const override;
		void bind() override;
	};
}