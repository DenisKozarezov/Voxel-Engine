#pragma once
#include "RenderSettings.h"
#include <Engine/Components/camera/Camera.h>

namespace utils
{
	class GizmosAPI;
}

struct RenderFrameStats
{
	uint32 drawCalls;
	uint64 triangles;
	uint64 vertices;
	uint64 primitives;
	uint64 clippingPrimitives;
	uint64 clippingInvocations;
	uint64 vertexShaderInvocations;
	uint64 fragmentShaderInvocations;
};

struct RenderPerformanceStats
{
	RenderFrameStats frameStats;
	uint32 voxels;
	uint32 batches;
	uint32 fps;
	double deltaTime;
};

namespace VoxelEngine
{
	class Window;
}

namespace VoxelEngine::renderer
{	
	class Renderer
	{
	private:
		friend class RenderCommand;
		
		static utils::GizmosAPI* s_gizmosAPI;
		static RenderPerformanceStats s_renderPerformanceStats;
		static RenderSettings s_renderSettings;
	public:
		FORCE_INLINE static RenderPerformanceStats& getStats() { return s_renderPerformanceStats; }
		FORCE_INLINE static RenderSettings& getRenderSettings() { return s_renderSettings; }
		static void resetStats();
		static void flushStats();
		
		static void init(const Window& window);
		static void preRender(const components::camera::Camera& camera);
		static void preRender();
		static void render();
		static void postRender();
		static void resize(const uint16& width, const uint16& height);
		static void deviceWaitIdle();
		static void shutdown();
	};
}