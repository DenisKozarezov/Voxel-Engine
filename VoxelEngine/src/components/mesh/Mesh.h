#pragma once
#include <array>
#include <core/renderer/Vertex.h>
#include <core/renderer/Buffer.h>

namespace VoxelEngine::components::mesh
{
	using Vertex = renderer::Vertex;

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

	class IMaterial
	{
	public:
		IMaterial() noexcept = default;

		virtual void bind() const = 0;

		virtual ~IMaterial() = default;
	};

	struct Mesh
	{
		Vertex* vertices = nullptr;
		uint32 vertexCount;
		uint32* indices = nullptr;
		uint32 indexCount;
		const IMaterial* material = nullptr;
		SharedRef<renderer::VertexBuffer> vertexBuffer;
		SharedRef<renderer::IndexBuffer> indexBuffer;

		Mesh() noexcept = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
			: Mesh(
				vertices.data(), 
				static_cast<uint32>(vertices.size()), 
				indices.data(), 
				static_cast<uint32>(indices.size())) { }
		
		Mesh(const Vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indexCount)
			: vertexCount(vertexCount), indexCount(indexCount)
		{
			this->vertices = new Vertex[vertexCount];
			memcpy(this->vertices, vertices, sizeof(Vertex) * vertexCount);
			
			this->indices = new uint32[indexCount];
			memcpy(this->indices, indices, sizeof(uint32) * indexCount);
		}
		~Mesh() noexcept = default;

		void release()
		{
			delete[] vertices;
			delete[] indices;

			if (vertexBuffer)
				vertexBuffer->release();

			if (indexBuffer)
				indexBuffer->release();
		}
	};

	struct TriangleMesh : public Mesh
	{
	private:
		static constexpr std::array<renderer::Vertex, 3> vertices =
		{
			Vertex({0.0f, -1.0f, 0.0f}),
			Vertex({1.0f, 1.0f, 0.0f}),
			Vertex({-1.0f, 1.0f, 0.0f})
		};
		static constexpr std::array<uint32, 3> indices = { 0, 1, 2 };
	public:
		TriangleMesh() : Mesh(
			vertices.data(), 
			static_cast<uint32>(vertices.size()), 
			indices.data(), 
			static_cast<uint32>(indices.size())) { }
	};

	struct QuadMesh : public Mesh
	{
		static constexpr std::array<renderer::Vertex, 4> vertices =
		{
			Vertex({-1.0f, -1.0f, 0.0f}),
			Vertex({1.0f, -1.0f, 0.0f}),
			Vertex({1.0f, 1.0f, 0.0f}),
			Vertex({-1.0f, 1.0f, 0.0f})
		};
		static constexpr std::array<uint32, 6> indices = { 0, 1, 2, 2, 3, 0 };

		QuadMesh() : Mesh(
			vertices.data(), 
			static_cast<uint32>(vertices.size()),
			indices.data(), 
			static_cast<uint32>(indices.size())) { }
	};

	struct VoxelMesh : public Mesh
	{
	private:
		static constexpr float s = 0.1f;
		
		static constexpr std::array<renderer::Vertex, 24> vertices =
		{
			Vertex({-s, -s, s},	{0.0f, 0.0f, 1.0f}),
			Vertex({s, -s, s},	{0.0f, 0.0f, 1.0f}),
			Vertex({-s, s, s},	{0.0f, 0.0f, 1.0f}),
			Vertex({s, s, s},	{0.0f, 0.0f, 1.0f}),

			Vertex({s, -s, -s},	{0.0f, 0.0f, -1.0f}),
			Vertex({-s, -s, -s},{0.0f, 0.0f, -1.0f}),
			Vertex({s, s, -s},	{0.0f, 0.0f, -1.0f}),
			Vertex({-s, s, -s},	{0.0f, 0.0f, -1.0f}),

			Vertex({-s, s, s},	{0.0f, 1.0f, 0.0f}),
			Vertex({s, s, s},	{0.0f, 1.0f, 0.0f}),
			Vertex({-s, s, -s},	{0.0f, 1.0f, 0.0f}),
			Vertex({s, s, -s},	{0.0f, 1.0f, 0.0f}),

			Vertex({-s, -s, -s},{0.0f, -1.0f, 0.0f}),
			Vertex({s, -s, -s},	{0.0f, -1.0f, 0.0f}),
			Vertex({-s, -s, s},	{0.0f, -1.0f, 0.0f}),
			Vertex({s, -s, s},	{0.0f, -1.0f, 0.0f}),

			Vertex({-s, -s, -s},{-1.0f, 0.0f, 0.0f}),
			Vertex({-s, -s, s},	{-1.0f, 0.0f, 0.0f}),
			Vertex({-s, s, -s},	{-1.0f, 0.0f, 0.0f}),
			Vertex({-s, s, s},	{-1.0f, 0.0f, 0.0f}),

			Vertex({s, -s, s},	{1.0f, 0.0f, 0.0f}),
			Vertex({s, -s, -s},	{1.0f, 0.0f, 0.0f}),
			Vertex({s, s, s},	{1.0f, 0.0f, 0.0f}),
			Vertex({s, s, -s},	{1.0f, 0.0f, 0.0f})
		};

		static constexpr std::array<uint32, 36> indices =
		{
			0, 1, 2,		2, 1, 3, 
			4, 5, 6,		6, 5, 7, 
			8, 9, 10,		10, 9, 11, 
			12, 13, 14,		14, 13, 15, 
			16, 17, 18,		18, 17, 19, 
			20, 21, 22,		22, 21, 23  
		};
	public:
		VoxelMesh() : Mesh(
			vertices.data(), 
			static_cast<uint32>(vertices.size()), 
			indices.data(), 
			static_cast<uint32>(indices.size())) { }
	};

	struct OptimizedVoxelMesh : public Mesh
	{
	private:
		static constexpr float s = 0.1f;

		static constexpr std::array<renderer::Vertex, 8> vertices =
		{
			Vertex({-s,-s, s}),
			Vertex({ s,-s, s}),
			Vertex({ s, s, s}),
			Vertex({-s, s, s}),
			Vertex({-s,-s,-s}),
			Vertex({ s,-s,-s}),
			Vertex({ s, s,-s}),
			Vertex({-s, s,-s}),
		};

		static constexpr std::array<uint32, 36> indices =
		{
			0,1,2, 2,3,0,
			1,5,6, 6,2,1,
			7,6,5, 5,4,7,
			4,0,3, 3,7,4,
			4,5,1, 1,0,4,
			3,2,6, 6,7,3,
		};
	public:
		OptimizedVoxelMesh() : Mesh(
			vertices.data(), 
			static_cast<uint32>(vertices.size()), 
			indices.data(), 
			static_cast<uint32>(indices.size())) { }
	};
}