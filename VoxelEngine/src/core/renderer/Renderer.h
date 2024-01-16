#pragma once
#include "RenderSettings.h"
#include "RenderCommand.h"
#include "GizmosAPI.h"
#include <components/camera/Camera.h>

namespace VoxelEngine::renderer
{
	struct RenderPerformanceStats
	{
		RenderFrameStats frameStats;
		ShaderStats shaderStats;
		uint32 voxels;
		uint32 batches;
		uint32 fps;
		float deltaTime;
	};

	static class Renderer
	{
	private:
		static utils::GizmosAPI* s_gizmosAPI;
		static RenderPerformanceStats s_renderPerformanceStats;
	public:
		static RenderSettings& getRenderSettings();
		static const RenderPerformanceStats& getStats();
		static void resetStats();
		static void flushStats();
		
		static void init(const Window& window);
		static void preRender(const components::camera::Camera& camera);
		static void preRender();
		static void render();
		static void postRender();
		static void resize(const uint32& width, const uint32& height);
		static void deviceWaitIdle();
		static void shutdown();
	};
}