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
		uint32 voxels;
		uint32 batches;
		uint32 fps;
		float deltaTime;
	};

	class Renderer
	{
	private:
		static UniqueRef<utils::GizmosAPI> s_gizmosAPI;
	public:
		static RenderSettings& getRenderSettings();
		static const RenderPerformanceStats& getStats();
		static void resetStats();
		static void init(const Window& window);
		static void preRender(const components::camera::Camera& camera);
		static void render();
		static void postRender();
		static void resize(const uint32& width, const uint32& height);
		static void deviceWaitIdle();
		static void shutdown();
	};
}