#pragma once
#include <core/PrimitiveTypes.h>
#include <core/Base.h>

namespace VoxelEngine::renderer
{
	class VertexBuffer
	{
	public:
		VertexBuffer() noexcept = default;
		virtual ~VertexBuffer() = default;

		virtual constexpr uint32 size() const = 0;

		virtual void setData(const void* data, const uint32& size) = 0;
		virtual void release() = 0;

		static SharedRef<VertexBuffer> Allocate(const uint32& size);
		static SharedRef<VertexBuffer> Allocate(const void* vertices, const uint32& size);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() noexcept = default;
		virtual ~IndexBuffer() = default;

		virtual constexpr uint32 size() const = 0;
		virtual void release() = 0;

		static SharedRef<IndexBuffer> Allocate(const void* indices, const uint32& size);
	};
}