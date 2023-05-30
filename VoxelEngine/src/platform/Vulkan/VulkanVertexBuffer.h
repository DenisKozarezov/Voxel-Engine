#pragma once
#include <core/PrimitiveTypes.h>
#include "VulkanVertex.h"

namespace VoxelEngine::renderer
{
	class VertexBuffer
	{
	private:
		VkBuffer _vertexBuffer;
		VkDeviceMemory _vertexBufferMemory;
		VkAllocationCallbacks* _allocator;
		VkDevice _logicalDevice;
	public:
		VertexBuffer() = default;
		VertexBuffer(const Vertex* vertices, const uint32& size, VkAllocationCallbacks* allocator = nullptr);

		const void bind() const;
		const void unbind() const;

		~VertexBuffer() = default;
	};	
}