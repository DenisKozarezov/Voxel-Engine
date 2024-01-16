#include "RenderCommand.h"
#include <Core/Logging/Assert.h>

namespace VoxelEngine::renderer
{
	UniqueRef<RendererAPI> RenderCommand::s_renderer = RendererAPI::Create();

	void RenderCommand::init(const Window& window)
	{
		s_renderer->init(window);
	}
	void RenderCommand::setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		VOXEL_CORE_ASSERT(x >= 0 && y >= 0, "invalid viewport size!");
		s_renderer->setViewport(x, y, width, height);
	}
	void RenderCommand::setClearColor(const glm::vec4& color)
	{
		s_renderer->setClearColor(color);
	}
	void RenderCommand::setLineWidth(const float& width)
	{
		VOXEL_CORE_ASSERT(width > 0.0f, "line width must be greater than zero!");
		s_renderer->setLineWidth(width);
	}
	void RenderCommand::draw(const mesh::IMaterial* material, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)
	{
		if (material)
			material->bind();

		s_renderer->draw(vertexCount, instanceCount, startVertex, startInstance);
	}
	void RenderCommand::drawMesh(const mesh::Mesh& mesh)
	{
		VOXEL_CORE_ASSERT(mesh.vertexBuffer, "can't draw mesh! Vertex buffer is empty!");

		if (mesh.material)
			mesh.material->bind();

		mesh.vertexBuffer->bind();
		s_renderer->drawMesh(mesh);
	}
	void RenderCommand::drawMeshIndexed(const mesh::Mesh& mesh, const SharedRef<renderer::IndexBuffer>& indexBuffer, uint32 indexCount, uint32 instanceCount, uint32 startInstance)
	{
		VOXEL_CORE_ASSERT(mesh.vertexBuffer, "can't draw mesh! Vertex buffer is empty!");
		VOXEL_CORE_ASSERT(indexBuffer->size() > 0, "can't draw mesh! Index buffer is empty!");

		if (mesh.material)
			mesh.material->bind();

		mesh.vertexBuffer->bind();
		indexBuffer->bind();
		s_renderer->drawMeshIndexed(indexCount, instanceCount, 0, startInstance);
	}
	void RenderCommand::drawMeshInstanced(const mesh::Mesh& mesh, const SharedRef<renderer::VertexBuffer>& instancedBuffer, uint32 instanceCount, uint32 startInstance)
	{
		if (mesh.material)
			mesh.material->bind();

		instancedBuffer->bind(1);
		drawMeshIndexed(mesh, instanceCount, startInstance);
	}
}