#pragma once
#include "RendererAPI.h"
#include <components/mesh/Mesh.h>

namespace VoxelEngine::renderer
{
	/* Facade class for rendering. */
	class RenderCommand
	{
	private:
		static UniqueRef<RendererAPI> s_renderer;
	public:
		static void init(const Window& window);
		static void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
		static void setClearColor(const glm::vec4& color);
		static void setLineWidth(const float& width);
		static void draw(const mesh::IMaterial* material, uint32 vertexCount, uint32 instanceCount = 1, uint32 startVertex = 0, uint32 startInstance = 0);
		static void drawMesh(const mesh::Mesh& mesh);
		static void drawMeshIndexed(const mesh::Mesh& mesh, uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawMeshIndexed(const mesh::Mesh& mesh, const SharedRef<renderer::IndexBuffer>& indexBuffer, uint32 indexCount, uint32 instanceCount = 1, uint32 startInstance = 0);
		static void drawMeshInstanced(const mesh::Mesh& mesh, const SharedRef<renderer::VertexBuffer>& instancedBuffer, uint32 instanceCount = 1, uint32 startInstance = 0);
	};

	INLINE void RenderCommand::drawMeshIndexed(const mesh::Mesh& mesh, uint32 instanceCount, uint32 startInstance)
	{
		drawMeshIndexed(mesh, mesh.indexBuffer, mesh.indexCount(), instanceCount, startInstance);
	}
}