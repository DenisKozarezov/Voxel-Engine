#pragma once
#include <glm/vec4.hpp>
#include <core/Window.h>

namespace VoxelEngine::renderer
{
	enum class GraphicsSpec : byte
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		DirectX12 = 3,
	};

	constexpr const string graphicsSpecString(const GraphicsSpec& spec)
	{
		switch (spec)
		{
#define STR(x) case GraphicsSpec::##x: return #x;
			STR(None);
			STR(Vulkan);
			STR(OpenGL);
			STR(DirectX12);
		default: return "UNKNOWN_RENDERER_SPEC";
		}
	}

	class RendererAPI
	{
	public:
		RendererAPI() noexcept = default;
		virtual ~RendererAPI() = default;

		virtual void init(const VoxelEngine::Window& window) = 0;
		virtual void setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void setLineWidth(const float& width) = 0;

		static UniqueRef<RendererAPI> Create();
	};

	extern const GraphicsSpec g_graphicsSpec;
}