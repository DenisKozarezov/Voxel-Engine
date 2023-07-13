#pragma once
#include <core/renderer/Buffers.h>
#include "VulkanVertex.h"

namespace vulkan
{
	class VulkanVertexBuffer : public VoxelEngine::renderer::VertexBuffer
	{
	private:
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
	public:
		VulkanVertexBuffer() = default;
		VulkanVertexBuffer(const void* vertices, const size_t& bufferSize);

		operator VkBuffer () const & { return _vertexBuffer; }

		void bind() const override;
		void release() const override;
		void setData(const void* data, const size_t& size) override;

		~VulkanVertexBuffer() = default;
	};	
}