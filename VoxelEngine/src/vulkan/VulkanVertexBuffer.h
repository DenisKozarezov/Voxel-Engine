#pragma once
#include <vulkan/VulkanAlloc.h>
#include <core/renderer/Buffers.h>

namespace vulkan
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		memory::Buffer _vertexBuffer;
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(const void* vertices, const size_t& bufferSize);

		operator VkBuffer () const & { return _vertexBuffer.buffer; }

		void bind() const override;
		void release() const override;
		void setData(const void* data, const size_t& size) override;

		~VulkanVertexBuffer() = default;
	};	
}