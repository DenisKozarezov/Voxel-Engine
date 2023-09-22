#pragma once
#include "RendererAPI.h"

namespace VoxelEngine::renderer
{
	class RenderCommand
	{
	private:
		static UniqueRef<RendererAPI> s_renderer;
	public:
		static void init(const Window& window);
		static void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
		static void setClearColor(const glm::vec4 color);
		static void setLineWidth(const float& width);
		static void drawMesh(const mesh::Mesh& mesh);
		static void drawMeshIndexed(const mesh::Mesh& mesh, uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawMeshIndexed(const mesh::Mesh& mesh, renderer::IndexBuffer& indexBuffer, uint32 indexCount, uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawMeshInstanced(const mesh::Mesh& mesh, renderer::VertexBuffer& instancedBuffer, uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawPrimitivesIndexed(const mesh::MeshTopology& topology, renderer::IndexBuffer& indexBuffer, uint32 indexCount, uint32 startIndex = 0, uint32 instanceCount = 1);
	};
}