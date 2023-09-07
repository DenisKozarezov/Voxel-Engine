#pragma once
#include "RenderSettings.h"
#include "RenderCommand.h"
#include <components/camera/Camera.h>
#include <core/renderer/UniformBuffer.h>

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
	public:
		static RenderSettings& getRenderSettings();
		static const RenderPerformanceStats& getStats();
		static void resetStats();
		static void init(const Window& window);
		static void preRender(const components::camera::Camera& camera);
		static void postRender();
		static void resize(const uint32& width, const uint32& height);
		static void submitRenderables(const std::vector<glm::vec3>& renderables);
		static void deviceWaitIdle();
		static void shutdown();
	};
}