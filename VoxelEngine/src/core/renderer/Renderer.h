#pragma once
#include "RenderContext.h"
#include "components/camera/Camera.h"
#include "RenderSettings.h"

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
		RenderFrameStats frameStats;
		uint32 voxels;
		uint32 batches;
		uint32 fps;
		float deltaTime;
	};

	class Renderer : public RenderContext
	{
	public:
		static RenderSettings& getRenderSettings();
		static const RenderPerformanceStats& getStats();
		void init(const Window& window) override;
		void preRender() override;
		void render() override;
		void postRender() override;
		void updateUIOverlay();
		void resize(const uint32& width, const uint32& height);
		static void setCamera(const components::camera::Camera& camera);
		static void submitRenderables(const std::vector<glm::vec3> objects);
		void deviceWaitIdle();
		void cleanup();
	};
}