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
	void RenderCommand::setClearColor(const glm::vec4& color)
	{
	}
	void RenderCommand::drawMesh(const mesh::Mesh& mesh)
	{

	}
	void RenderCommand::drawMeshInstanced(const mesh::Mesh& mesh, InstanceData instanceData[], uint32 instanceCount, uint32 startInstance)
	{

	}
	void RenderCommand::drawPrimitivesIndexed(const mesh::MeshTopology& type, uint32 indexBuffer[], uint32 indexCount, uint32 startIndex, uint32 instanceCount)
	{

	}
}