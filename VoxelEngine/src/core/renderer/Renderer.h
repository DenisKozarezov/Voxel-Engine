#pragma once
#include <core/PrimitiveTypes.h>

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

	};
}