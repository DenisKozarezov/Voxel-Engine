#pragma once
#include <list>
#include <core/PrimitiveTypes.h>
#include <vulkan/VulkanVertex.h>

namespace VoxelEngine::components::mesh
{
	enum MeshType
	{
		Triangle = 0,
		Square = 1,
		Polygone = 2,
	};

	struct Mesh
	{
		std::list<vulkan::Vertex> vertices;
		std::list<uint32> indices;

		Mesh() noexcept = default;

		~Mesh()
		{

		}
	};
}