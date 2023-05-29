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
		static const float getTime();
		static const void init(const SharedRef<Window> window);
		static const void beginFrame();
		static const void endFrame();
		static const void deviceWaitIdle();
		static const void cleanup();
	};
}