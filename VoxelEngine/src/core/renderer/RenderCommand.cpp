#include "RenderCommand.h"

namespace VoxelEngine::renderer
{
	UniqueRef<RendererAPI> RenderCommand::s_renderer = RendererAPI::Create();

	void RenderCommand::setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height)
	{
		s_renderer->setViewport(x, y, width, height);
	}
	void RenderCommand::setClearColor(const glm::vec4& color)
	{
	}
}