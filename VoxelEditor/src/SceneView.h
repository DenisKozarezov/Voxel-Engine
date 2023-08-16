#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	using namespace VoxelEngine;

	class SceneView final
	{
	private:
		ImVec2 m_viewportSize;

		void drawRenderModes();
		void drawCameraModes();
	public:
		UniqueRef<VoxelEngine::components::camera::EditorCameraController> m_camera;
		bool m_viewportFocused;
		bool m_viewportHovered;

		SceneView();
		~SceneView() noexcept = default;
		SceneView(SceneView const&) noexcept = delete;
		SceneView(SceneView&&) noexcept = delete;
		SceneView& operator=(SceneView const& rhs) noexcept = delete;
		SceneView& operator=(SceneView&& rhs) noexcept = delete;

		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);

		void onImGuiRender();
		void update(const Timestep& ts);
	};
}