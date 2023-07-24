#pragma once
#include "../Window.h"
#include "components/camera/Camera.h"

namespace VoxelEngine::renderer
{
	enum class RendererAPI : byte
	{
		None = 0,
		OpenGL = 1,
		Vulkan = 2,
		DirectX12 = 3,
	};

	struct RenderPerformanceStats
	{
		uint32 drawCalls;
		uint64 triangles;
		uint64 vertices;
		uint64 indices;
		uint32 voxels;
		uint32 batches;
		float fps;
		float deltaTime;
	};

	class Renderer
	{
	public:
		static const RenderPerformanceStats& getStats();
		static const float getTime();
		static void init(const Window& window);
		static void beginFrame();
		static void endFrame();
		static void deviceWaitIdle();
		static void cleanup();
	};
}