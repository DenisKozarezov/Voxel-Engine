#pragma once
#include <vector>
#include <core/PrimitiveTypes.h>
#include <vulkan/VulkanVertex.h>

namespace VoxelEngine::components::mesh
{
	enum MeshType
	{
		Triangle = 0,
		Square = 1,
		Polygone = 2,
		Cube = 3
	};

	struct Mesh
	{
		std::vector<vulkan::Vertex> vertices;
		std::vector<vulkan::Vertex> normals;
		std::vector<uint32> indices;

		Mesh() noexcept = default;
		~Mesh() noexcept = default;
	};
}