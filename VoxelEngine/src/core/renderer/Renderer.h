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
		static float getTime() noexcept;
		static void init(const Window& window);
		static void beginFrame();
		static void endFrame();
		static void deviceWaitIdle();
		static void cleanup();
	};
}