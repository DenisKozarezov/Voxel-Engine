#pragma once
#include <vector>
#include <core/renderer/Buffers.h>

namespace VoxelEngine::components::mesh
{
	struct Mesh
	{
		std::vector<vulkan::Vertex> vertices;
		std::vector<uint16> indices;
		renderer::VertexBuffer* vertexBuffer;
		renderer::IndexBuffer* indexBuffer;
	};
}