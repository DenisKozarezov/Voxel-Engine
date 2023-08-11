#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	using namespace VoxelEngine;

	class SceneView final
	{
	private:
		float _lastMouseX = 0.0f, _lastMouseY = 0.0f;
		glm::vec2 _viewportSize;
		input::MouseDraggingState _mouseState;

		void drawRenderModes();
	public:
		UniqueRef<components::camera::Camera> _camera;
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
		bool onMouseMoved(const input::MouseMovedEvent& e);
		void render();

		void setMouseDragging(const bool& isDragging);
		void moveCamera(const components::camera::CameraMovement& direction, const float& deltaTime);
		void mouseMove(const float& x, const float& y);
	};
}