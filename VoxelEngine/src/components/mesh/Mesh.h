#pragma once
#include <core/renderer/Vertex.h>
#include <core/renderer/Buffer.h>

namespace VoxelEngine::components::mesh
{
	using Vertex = renderer::Vertex;

	class IMaterial
	{
	public:
		IMaterial() noexcept = default;

		virtual void bind() const = 0;

		virtual ~IMaterial() = default;
	};

	struct Mesh
	{
	private:
		void release()
		{
			if (vertexBuffer)
				vertexBuffer->release();

			if (indexBuffer)
				indexBuffer->release();
		}
	public:
		std::vector<Vertex> vertices;
		std::vector<uint32> indices;
		const IMaterial* material = nullptr;
		SharedRef<renderer::VertexBuffer> vertexBuffer;
		SharedRef<renderer::IndexBuffer> indexBuffer;

		Mesh() noexcept = default;
		Mesh(const std::vector<Vertex>& vertices, const std::vector<uint32>& indices)
		{
			this->vertices.assign(vertices.begin(), vertices.end());
			this->indices.assign(indices.begin(), indices.end());
		}
		Mesh(const Vertex* vertices, uint32 vertexCount, const uint32* indices, uint32 indexCount)
		{
			std::copy(vertices, vertices + vertexCount, std::back_inserter(this->vertices));
			std::copy(indices, indices + indexCount, std::back_inserter(this->indices));
		}
		Mesh(const Mesh& rhs) : Mesh(rhs.vertices, rhs.indices)
		{
			this->vertexBuffer = rhs.vertexBuffer;
			this->indexBuffer = rhs.indexBuffer;
		}
		Mesh(Mesh&& rhs) noexcept
		{
			this->vertices.swap(rhs.vertices);
			this->indices.swap(rhs.indices);
			this->material = rhs.material;
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
		}
		Mesh& operator=(const Mesh& rhs)
		{		
			if (this == &rhs)
				return *this;

			release();

			this->vertices.assign(rhs.vertices.begin(), rhs.vertices.end());
			this->indices.assign(rhs.indices.begin(), rhs.indices.end());
			this->material = rhs.material;
			this->vertexBuffer = rhs.vertexBuffer;
			this->indexBuffer = rhs.indexBuffer;

			return *this;
		}
		Mesh& operator=(Mesh&& rhs) noexcept
		{
			if (this == &rhs)
				return *this;

			this->vertices.swap(rhs.vertices);
			this->indices.swap(rhs.indices);
			this->material = rhs.material;
			this->vertexBuffer.swap(rhs.vertexBuffer);
			this->indexBuffer.swap(rhs.indexBuffer);
			
			return *this;
		}

		INLINE uint32 indexCount() { return static_cast<uint32>(indices.size()); }
		INLINE uint32 vertexCount() { return static_cast<uint32>(vertices.size()); }
		INLINE const uint32 indexCount() const { return static_cast<uint32>(indices.size()); }
		INLINE const uint32 vertexCount() const { return static_cast<uint32>(vertices.size()); }

		~Mesh() noexcept
		{
			release();
		}
	};
}