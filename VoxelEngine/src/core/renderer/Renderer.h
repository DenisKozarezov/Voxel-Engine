#pragma once
#include "../Window.h"

namespace VoxelEngine::renderer
{
	enum class RendererAPI : byte
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		DirectX12 = 3,
	};

	class Renderer
	{
	public:
		virtual const float getTime() const noexcept = 0;
		virtual const void setWindow(const UniqueRef<Window>& window) noexcept = 0;
		virtual const void init() = 0;
		//virtual const void renderFrame() = 0;
		virtual const void deviceWaitIdle() const = 0;
		virtual const void cleanup() = 0;
	};
}