#pragma once
#include "RendererAPI.h"

namespace VoxelEngine::renderer
{
	class RenderCommand
	{
	private:
		static UniqueRef<RendererAPI> s_renderer;
	public:
		static void setViewport(const int32_t& x, const int32_t& y, const uint32& width, const uint32& height);
		static void setClearColor(const glm::vec4& color);
	};
}