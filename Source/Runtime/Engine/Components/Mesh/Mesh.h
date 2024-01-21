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
		const Vertex* vertices = nullptr;
		const uint32* indices = nullptr;
		TSharedPtr<renderer::VertexBuffer> vertexBuffer = nullptr;
		TSharedPtr<renderer::IndexBuffer> indexBuffer = nullptr;
		TSharedPtr<const renderer::IMaterial> material = nullptr;
		
		Mesh() noexcept = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
			: Mesh(vertices.data(), vertices.size(), indices.data(), indices.size())
		{ }
		Mesh(const Vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indexCount)
		{
			this->vertices = vertices;
			this->indices = indices;
			
			vertexBuffer = renderer::VertexBuffer::Allocate(vertices, vertexCount * sizeof(renderer::Vertex));
			indexBuffer = renderer::IndexBuffer::Allocate(indices, indexCount * sizeof(uint32));
		}
		Mesh(const Mesh& rhs) noexcept = delete;
		Mesh(Mesh&& rhs) noexcept
		{
			this->material = rhs.material;
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
		}
		Mesh& operator=(const Mesh& rhs)
		{		
			if (this == &rhs)
				return *this;

			release();
			
			this->material = rhs.material;
			this->vertexBuffer = rhs.vertexBuffer;
			this->indexBuffer = rhs.indexBuffer;

			return *this;
		}
		Mesh& operator=(Mesh&& rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			release();
			
			this->material = rhs.material;
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
			
			return *this;
		}

		FORCE_INLINE uint32 indexCount() { return indexBuffer->size(); }
		FORCE_INLINE uint32 vertexCount() { return vertexBuffer->size(); }
		FORCE_INLINE uint32 indexCount() const { return indexBuffer->size(); }
		FORCE_INLINE uint32 vertexCount() const { return vertexBuffer->size(); }
		
		~Mesh()
		{
			release();
		}
	};
}