#include "RenderCommand.h"
#include <core/Assert.h>

namespace VoxelEngine::renderer
{
	UniqueRef<RendererAPI> RenderCommand::s_renderer = RendererAPI::Create();

	void RenderCommand::init(const Window& window)
	{
		s_renderer->init(window);
	}
	void RenderCommand::setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		bool invalidSize = x < 0 || y < 0;
		VOXEL_CORE_ASSERT(!invalidSize, "Invalid viewport size!");
		s_renderer->setViewport(x, y, width, height);
	}
	void RenderCommand::setClearColor(const glm::vec4 color)
	{
		s_renderer->setClearColor(color);
	}
	void RenderCommand::setLineWidth(const float& width)
	{
		bool invalidWidth = width <= 0.0f;
		VOXEL_CORE_ASSERT(!invalidWidth, "Line width must be greater than zero!");
		s_renderer->setLineWidth(width);
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
	void RenderCommand::drawPrimitivesIndexed(const mesh::MeshTopology& topology, renderer::IndexBuffer& indexBuffer, uint32 indexCount, uint32 startIndex, uint32 instanceCount)
	{
		//drawMeshIndexed(topologyMesh, indexBuffer, indexCount, startIndex, instanceCount);
	}
}