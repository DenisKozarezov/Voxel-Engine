#pragma once
#include <Renderer/Vertex.h>
#include <Renderer/Buffer.h>
#include <Renderer/IMaterial.h>

namespace VoxelEngine::components::mesh
{
	using Vertex = renderer::Vertex;
	
	struct Mesh
	{
	private:		
		void release()
		{
			vertexBuffer = nullptr;
			indexBuffer = nullptr;
		}
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;
		TSharedPtr<renderer::VertexBuffer> vertexBuffer = nullptr;
		TSharedPtr<renderer::IndexBuffer> indexBuffer = nullptr;
		TSharedPtr<const renderer::IMaterial> material = nullptr;
		
		Mesh() noexcept = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
			: Mesh(vertices.data(), vertices.size(), indices.data(), indices.size())
		{ }
		Mesh(const Vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indexCount)
		{
			std::copy_n(vertices, vertexCount, std::back_inserter(this->vertices));
			std::copy_n(indices, indexCount, std::back_inserter(this->indices));
			
			vertexBuffer = renderer::VertexBuffer::Allocate(vertices, vertexCount * sizeof(renderer::Vertex));
			indexBuffer = renderer::IndexBuffer::Allocate(indices, indexCount * sizeof(uint32));
		}
		Mesh(const Mesh& rhs) : Mesh(rhs.vertices, rhs.indices) { }
		Mesh(Mesh&& rhs) noexcept
		{
			this->vertices.swap(rhs.vertices);
			this->indices.swap(rhs.indices);
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
			this->material.swap(rhs.material);
		}
		Mesh& operator=(const Mesh& rhs)
		{		
			if (this == &rhs)
				return *this;

			release();

			this->vertices.assign(rhs.vertices.begin(), rhs.vertices.end());
			this->indices.assign(rhs.indices.begin(), rhs.indices.end());
			this->vertexBuffer = rhs.vertexBuffer;
			this->indexBuffer = rhs.indexBuffer;
			this->material = rhs.material;

			return *this;
		}
		Mesh& operator=(Mesh&& rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			release();

			this->vertices.swap(rhs.vertices);
			this->indices.swap(rhs.indices);
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
			this->material.swap(rhs.material);
			
			return *this;
		}

		FORCE_INLINE uint32 indexCount() { return static_cast<uint32>(indices.size()); }
		FORCE_INLINE uint32 vertexCount() { return static_cast<uint32>(vertices.size()); }
		FORCE_INLINE uint32 indexCount() const { return static_cast<uint32>(indices.size()); }
		FORCE_INLINE uint32 vertexCount() const { return static_cast<uint32>(vertices.size()); }
		
		~Mesh()
		{
			release();
		}
	};
}