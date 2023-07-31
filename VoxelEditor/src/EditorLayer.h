#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	using namespace VoxelEngine;

	class EditorLayer : public VoxelEngine::renderer::Layer
	{
	private:
		input::MouseDraggingState _mouseState;
		input::EventDispatcher _dispatcher;
		components::camera::FirstPersonCamera _camera{ glm::vec3(2.0f, 2.0f, 2.0f) };
		float _lastMouseX = 0.0f, _lastMouseY = 0.0f;
		float _deltaTime = 0.0f;
		float _fixedDeltaTime = 0.0f;

		bool onKeyboardPressed(const input::KeyPressedEvent& e);
		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);
		bool onMouseMoved(const input::MouseMovedEvent& e);

		void setMouseDragging(const bool& isDragging);
		void moveCamera(const components::camera::CameraMovement& direction);
		void mouseMove(const float& x, const float& y);
		void drawMenuBar();
		void drawRenderModes();
		void drawRenderPerformance();
	public:
		EditorLayer();
		~EditorLayer() = default;

		void onAttach() override;
		void onDetach() override;
		void onUpdate(const VoxelEngine::Timestep& ts) override;
		void onFixedUpdate(const VoxelEngine::Timestep& ts) override;
		void onImGuiRender() override;
		void onEvent(VoxelEngine::input::Event& e) override;
	};
}