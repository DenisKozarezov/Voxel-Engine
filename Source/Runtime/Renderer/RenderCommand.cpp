#include "RenderCommand.h"
#include <Core/Logging/Assert.h>

namespace VoxelEngine::renderer
{
	TUniquePtr<RendererAPI> RenderCommand::s_renderer = RendererAPI::Create();

	void RenderCommand::init(const Window& window)
	{
		s_renderer->init(window);
	}
	void RenderCommand::setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		RUNTIME_ASSERT(x >= 0 && y >= 0, "invalid viewport size!");
		s_renderer->setViewport(x, y, width, height);
	}
	void RenderCommand::setClearColor(const glm::vec4& color)
	{
		s_renderer->setClearColor(color);
	}
	void RenderCommand::setLineWidth(const float& width)
	{
		RUNTIME_ASSERT(width > 0.0f, "line width must be greater than zero!");
		s_renderer->setLineWidth(width);
	}
	void RenderCommand::draw(const IMaterial* material, uint32 vertexCount, uint32 instanceCount, uint32 startVertex, uint32 startInstance)
	{
		if (material)
			material->bind();

		s_renderer->draw(vertexCount, instanceCount, startVertex, startInstance);
	}
	void RenderCommand::drawMesh(const Mesh& mesh)
	{
		RUNTIME_ASSERT(mesh.vertexBuffer && !mesh.vertexBuffer->empty(), "can't draw mesh! Vertex buffer is empty!");

		if (mesh.material)
			mesh.material->bind();

		mesh.vertexBuffer->bind();
		s_renderer->drawMesh(mesh);
	}
	void RenderCommand::drawMeshIndexed(const Mesh& mesh, const TSharedPtr<IndexBuffer>& indexBuffer, uint32 indexCount, uint32 instanceCount, uint32 startInstance)
	{
		RUNTIME_ASSERT(mesh.vertexBuffer && !mesh.vertexBuffer->empty(), "can't draw mesh! Vertex buffer is empty!");
		RUNTIME_ASSERT(mesh.indexBuffer && !indexBuffer->empty(), "can't draw mesh! Index buffer is empty!");

		if (mesh.material)
			mesh.material->bind();

		mesh.vertexBuffer->bind();
		indexBuffer->bind();
		s_renderer->drawMeshIndexed(indexCount, instanceCount, 0, startInstance);
	}
	void RenderCommand::drawMeshInstanced(const Mesh& mesh, const TSharedPtr<VertexBuffer>& instancedBuffer, uint32 instanceCount, uint32 startInstance)
	{
		RUNTIME_ASSERT(instancedBuffer && !instancedBuffer->empty(), "can't draw instanced mesh! Instanced buffer is empty!");
		
		if (mesh.material)
			mesh.material->bind();

		instancedBuffer->bind(1, 2);
		drawMeshIndexed(mesh, instanceCount, startInstance);
	}
}