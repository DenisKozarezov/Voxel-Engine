#pragma once
#include <core/renderer/Buffers.h>

namespace VoxelEngine::components::mesh
{
	struct Mesh
	{
		std::list<vulkan::Vertex> vertices;
		std::list<uint32> indices;
		renderer::VertexBuffer* vertexBuffer;
		renderer::IndexBuffer* indexBuffer;

		Mesh() noexcept = default;

		~Mesh()
		{
			vertexBuffer->release();
			indexBuffer->release();
		}
	};
}