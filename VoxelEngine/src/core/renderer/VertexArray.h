#pragma once
#include <pch.h>
#include <core/Base.h>
#include "Buffers.h"

namespace VoxelEngine::renderer
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void bind() const = 0;
		virtual void unbind() const = 0;

		virtual void addVertexBuffer(const SharedRef<VertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const SharedRef<IndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<SharedRef<VertexBuffer>>& getVertexBuffers() const = 0;
		virtual const SharedRef<IndexBuffer>& getIndexBuffer() const = 0;

		static SharedRef<VertexArray> Create();
	};
}
