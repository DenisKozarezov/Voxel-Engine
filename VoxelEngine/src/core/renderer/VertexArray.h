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

		virtual void addVertexBuffer(const SharedRef<VulkanVertexBuffer>& vertexBuffer) = 0;
		virtual void setIndexBuffer(const SharedRef<VulkanIndexBuffer>& indexBuffer) = 0;

		virtual const std::vector<SharedRef<VulkanVertexBuffer>>& getVertexBuffers() const = 0;
		virtual const SharedRef<VulkanIndexBuffer>& getIndexBuffer() const = 0;

		static SharedRef<VertexArray> Create();
	};
}
