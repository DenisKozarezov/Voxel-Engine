#pragma once
#include "../Window.h"

namespace VoxelEngine
{
	class RenderContext
	{
	public:
		RenderContext() noexcept = default;
		RenderContext(RenderContext const&) noexcept = delete;
		RenderContext(RenderContext&&) noexcept = delete;
		RenderContext& operator=(RenderContext const& rhs) noexcept = delete;
		RenderContext& operator=(RenderContext&& rhs) noexcept = delete;
		~RenderContext() noexcept = default;

		virtual void init(const Window& window) = 0;
		virtual void preRender() = 0;
		virtual void render() = 0;
		virtual void postRender() = 0;
	};
}