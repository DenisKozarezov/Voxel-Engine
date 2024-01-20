#pragma once
#include <Core/CoreTypes.h>
#include <Core/CoreDefines.h>

namespace VoxelEngine::renderer
{
	class VertexBuffer
	{
	public:
		VertexBuffer() noexcept = default;
		virtual ~VertexBuffer() = default;

		NODISCARD virtual constexpr uint32 size() const = 0;

		virtual void setData(const void* data, const uint32& size) = 0;
		virtual void bind(const uint32& binding = 0) = 0;
		virtual void release() = 0;

		static TSharedPtr<VertexBuffer> Allocate(const uint32& size);
		static TSharedPtr<VertexBuffer> Allocate(const void* vertices, const uint32& size);
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() noexcept = default;
		virtual ~IndexBuffer() = default;

		NODISCARD virtual constexpr uint32 size() const = 0;
		virtual void bind() = 0;
		virtual void release() = 0;

		static TSharedPtr<IndexBuffer> Allocate(const uint32& size);
		static TSharedPtr<IndexBuffer> Allocate(const void* indices, const uint32& size);
	};
}