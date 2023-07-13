#include "Buffers.h"
#include "vulkan/VulkanVertexBuffer.h"
#include "vulkan/VulkanIndexBuffer.h"
#include "vulkan/VulkanUniformBuffer.h"

namespace VoxelEngine::renderer
{
	IndexBuffer* IndexBuffer::Create(const void* data, const size_t& size)
	{
		return new vulkan::VulkanIndexBuffer(data, size);
	}

	VertexBuffer* VertexBuffer::Create(const void* data, const size_t& size)
	{
		return new vulkan::VulkanVertexBuffer(data, size);
	}

	UniformBuffer* UniformBuffer::Create(const size_t& size)
	{
		return new vulkan::VulkanUniformBuffer(size);
	}
}