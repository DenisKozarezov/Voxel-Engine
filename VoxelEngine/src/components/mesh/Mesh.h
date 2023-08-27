#pragma once
#include <vector>
#include <core/PrimitiveTypes.h>
#include <core/renderer/Vertex.h>

using Vertex = VoxelEngine::renderer::Vertex;

namespace VoxelEngine::components::mesh
{
	enum MeshTopology
	{
		Triangle = 0,
		Quad = 1,
		Line = 2,
		LineStrip = 3,
		Points = 4,
		Polygone = 5,
		Cube = 6
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
			{{0.0f, -0.5f, 0.0f},	{1.0f, 1.0f, 1.0f}},
			{{0.5f, 0.5f, 0.0f},	{0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f},	{0.0f, 0.0f, 1.0f}}
		};
		const std::vector<uint32> indices = { 0, 1, 2 };
	};

	struct QuadMesh
	{
		const std::vector<renderer::Vertex> vertices =
		{
			{{-1.0f, -1.0f, 0.0f}},
			{{1.0f, -1.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}},
			{{-1.0f, 1.0f, 0.0f}}
		};
		const std::vector<uint32> indices = { 0, 1, 2, 2, 3, 0 };
	};

	struct VoxelMesh
	{
		float s;
		
		constexpr VoxelMesh(float size = 0.1f) : s(size) { }

		const std::vector<renderer::Vertex> vertices =
		{
			{{-s, -s, s},	{0.0f, 0.0f, 1.0f}},
			{{s, -s, s},	{0.0f, 0.0f, 1.0f}},
			{{-s, s, s},	{0.0f, 0.0f, 1.0f}},
			{{s, s, s},		{0.0f, 0.0f, 1.0f}},

			{{s, -s, -s},	{0.0f, 0.0f, -1.0f}},
			{{-s, -s, -s},	{0.0f, 0.0f, -1.0f}},
			{{s, s, -s},	{0.0f, 0.0f, -1.0f}},
			{{-s, s, -s},	{0.0f, 0.0f, -1.0f}},

			{{-s, s, s},	{0.0f, 1.0f, 0.0f}},
			{{s, s, s},		{0.0f, 1.0f, 0.0f}},
			{{-s, s, -s},	{0.0f, 1.0f, 0.0f}},
			{{s, s, -s},	{0.0f, 1.0f, 0.0f}},

			{{-s, -s, -s},	{0.0f, -1.0f, 0.0f}},
			{{s, -s, -s},	{0.0f, -1.0f, 0.0f}},
			{{-s, -s, s},	{0.0f, -1.0f, 0.0f}},
			{{s, -s, s},	{0.0f, -1.0f, 0.0f}},

			{{-s, -s, -s},	{-1.0f, 0.0f, 0.0f}},
			{{-s, -s, s},	{-1.0f, 0.0f, 0.0f}},
			{{-s, s, -s},	{-1.0f, 0.0f, 0.0f}},
			{{-s, s, s},	{-1.0f, 0.0f, 0.0f}},

			{{s, -s, s},	{1.0f, 0.0f, 0.0f}},
			{{s, -s, -s},	{1.0f, 0.0f, 0.0f}},
			{{s, s, s},		{1.0f, 0.0f, 0.0f}},
			{{s, s, -s},	{1.0f, 0.0f, 0.0f}}
		};

		const std::vector<uint32> indices =
		{
			0, 1, 2,		2, 1, 3, 
			4, 5, 6,		6, 5, 7, 
			8, 9, 10,		10, 9, 11, 
			12, 13, 14,		14, 13, 15, 
			16, 17, 18,		18, 17, 19, 
			20, 21, 22,		22, 21, 23  
		};
	};

	struct OptimizedVoxelMesh
	{
		float s = 1.0f;

		std::vector<renderer::Vertex> vertices =
		{
			{{-s,-s, s}},
			{{ s,-s, s}},
			{{ s, s, s}},
			{{-s, s, s}},
			{{-s,-s,-s}},
			{{ s,-s,-s}},
			{{ s, s,-s}},
			{{-s, s,-s}},
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
		
		constexpr OptimizedVoxelMesh(float size = 0.1f) : s(size)
		{
			VoxelMesh notOptimizedMesh = VoxelMesh(size);
			auto notOptimizedVertices = notOptimizedMesh.vertices;

			for (size_t i = 0; i < notOptimizedVertices.size(); i += 4)
			{
				glm::vec3 v0 = notOptimizedVertices[i + 0].pos;
				glm::vec3 v1 = notOptimizedVertices[i + 1].pos;
				glm::vec3 v2 = notOptimizedVertices[i + 2].pos;

				const glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

				notOptimizedVertices[i + 0].normal = normal;
				notOptimizedVertices[i + 1].normal = normal;
				notOptimizedVertices[i + 2].normal = normal;
				notOptimizedVertices[i + 3].normal = normal;
			}

			int j = 0;
			for (size_t i = 0; i < notOptimizedVertices.size(); i += 4, j++)
			{
				vertices[j].normal = notOptimizedVertices[i].normal;
			}
		}
	};
}