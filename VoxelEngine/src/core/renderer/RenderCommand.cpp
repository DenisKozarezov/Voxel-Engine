#include "RenderCommand.h"

namespace VoxelEngine::renderer
{
	UniqueRef<RendererAPI> RenderCommand::s_renderer = RendererAPI::Create();

	void RenderCommand::init(const Window& window)
	{
		s_renderer->init(window);
	}
	void RenderCommand::setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		s_renderer->setViewport(x, y, width, height);
	}
	void RenderCommand::setClearColor(const glm::vec4 color)
	{
		s_renderer->setClearColor(color);
	}
	void RenderCommand::drawMesh(const mesh::Mesh& mesh)
	{
		mesh.vertexBuffer->bind();
		s_renderer->drawMesh(mesh);
	}
	void RenderCommand::drawMeshIndexed(const mesh::Mesh& mesh, uint32 instanceCount, uint32 startInstance)
	{
		mesh.vertexBuffer->bind();
		mesh.indexBuffer->bind();
		s_renderer->drawMeshIndexed(mesh, instanceCount, startInstance);
	}
	void RenderCommand::drawMeshIndexed(const mesh::Mesh& mesh, renderer::IndexBuffer& indexBuffer, uint32 indexCount, uint32 instanceCount, uint32 startInstance)
	{
		mesh.vertexBuffer->bind();
		indexBuffer.bind();
		s_renderer->drawMeshIndexed(indexCount, instanceCount, startInstance);
	}
	void RenderCommand::drawMeshInstanced(const mesh::Mesh& mesh, InstanceData instanceData[], uint32 instanceCount, uint32 startInstance)
	{
		mesh.vertexBuffer->bind();
		mesh.indexBuffer->bind();
		s_renderer->drawMeshInstanced(mesh, instanceData, instanceCount, startInstance);
	}
}