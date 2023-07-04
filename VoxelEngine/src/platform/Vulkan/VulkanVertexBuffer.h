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
		VkDevice _logicalDevice;
	public:
		VertexBuffer() = default;
		VertexBuffer(const VkDevice& logicalDevice, const Vertex* vertices, const uint32& bufferSize);

		operator VkBuffer () const & { return _vertexBuffer; }

		void bind(const VkCommandBuffer& commandBuffer) const;
		void release() const;

		~VertexBuffer() = default;
	};	
}