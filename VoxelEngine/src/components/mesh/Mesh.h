#pragma once
#include <vector>
#include <core/PrimitiveTypes.h>
#include <core/renderer/Vertex.h>

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
		std::vector<renderer::Vertex> vertices;
		std::vector<renderer::Vertex> normals;
		std::vector<uint32> indices;

		Mesh() noexcept = default;
		~Mesh() noexcept = default;
	};

	struct TriangleMesh
	{
		const std::vector<renderer::Vertex> vertices =
		{
			{{0.0f, -0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};
		const std::vector<uint32> indices = { 0, 1, 2 };
	};

	struct SquareMesh
	{
		const std::vector<renderer::Vertex> vertices =
		{
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}}
		};
		const std::vector<uint32> indices = { 0, 1, 2, 2, 3, 0 };
	};

	struct VoxelMesh
	{
		static constexpr float s = 0.1f;
		const std::vector<renderer::Vertex> vertices =
		{
			{{-s,-s, s}, {1.0f, 0.0f, 0.0f }},
			{{ s,-s, s}, {0.0f, 1.0f, 0.0f }},
			{{ s, s, s}, {0.0f, 0.0f, 1.0f }},
			{{-s, s, s}, {0.0f, 0.0f, 0.0f }},
			{{-s,-s,-s}, {1.0f, 0.0f, 0.0f }},
			{{ s,-s,-s}, {0.0f, 1.0f, 0.0f }},
			{{ s, s,-s}, {0.0f, 0.0f, 1.0f }},
			{{-s, s,-s}, {0.0f, 0.0f, 0.0f }},
		};

		const std::vector<uint32> indices =
		{
			0,1,2, 2,3,0, 
			1,5,6, 6,2,1, 
			7,6,5, 5,4,7, 
			4,0,3, 3,7,4, 
			4,5,1, 1,0,4, 
			3,2,6, 6,7,3,
		};
	};
}