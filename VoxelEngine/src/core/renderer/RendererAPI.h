#pragma once
#include <core/PrimitiveTypes.h>
#include <glm/vec4.hpp>
#include <core/Base.h>

namespace VoxelEngine::renderer
{
	class RendererAPI
	{
	public:
		enum class API : byte
		{
			None = 0,
			OpenGL = 1,
			Vulkan = 2,
			DirectX12 = 3,
		};

		RendererAPI() noexcept = default;
		virtual ~RendererAPI() = default;

		virtual void init() = 0;
		virtual void setViewport(const uint32& x, const uint32& y, const uint32& width, const uint32& height) = 0;
		virtual void setClearColor(const glm::vec4& color) = 0;
		virtual void setLineWidth(const float& width) = 0;

		static UniqueRef<RendererAPI> Create();
	};

	extern const RendererAPI::API g_rendererAPI;
}