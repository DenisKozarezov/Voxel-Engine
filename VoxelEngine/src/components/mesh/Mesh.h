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
			// 0 - 3
			{{ -s, s, s}, {0.0f, 1.0f, 1.0f}},
			{{ -s, -s, s}, {0.0f, 0.0f, 1.0f}},
			{{ s, s, s}, {1.0f, 1.0f, 1.0f}},
			{{ s, -s, s}, {1.0f, 0.0f, 1.0f}},

			// 4 - 5
			{{ s, s, -s}, {1.0f, 1.0f, 0.0f}},
			{{ s, -s, -s}, {1.0f, 0.0f, 0.0f}},

			// 6 - 7
			{{-s, s, -s}, {0.0f, 1.0f, 0.0f}},
			{{ -s, -s, -s}, {0.0f, 0.0f, 0.0f}},
		};

		const std::vector<uint32> indices =
		{
			// Front
			0,1,2, 1,3,2,

			// Right
			4,2,3, 4,3,5,

			// Back
			4,5,6, 5,7,6,

			// Right
			6,7,0, 1,0,7,

			// Top
			6,0,4, 0,2,4,

			// Bottom
			1,7,5, 1,5,3
		};
	};
}