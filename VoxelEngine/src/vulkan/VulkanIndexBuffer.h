#pragma once
#include <vulkan/VulkanAlloc.h>
#include <core/renderer/Buffers.h>

namespace vulkan
{
	class VulkanIndexBuffer : public VoxelEngine::renderer::IndexBuffer
	{
	private:
		memory::Buffer _indexBuffer;
	public:
		VulkanIndexBuffer() = default;
		VulkanIndexBuffer(const void* indices, const size_t& bufferSize);

		operator const VkBuffer () const & { return _indexBuffer.buffer; }

		void bind() const override;
		void release() const override;
		void setData(const void* data, const size_t& size) override;

		~VulkanIndexBuffer() = default;
	};
}