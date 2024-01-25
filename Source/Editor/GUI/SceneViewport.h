#pragma once
#include "ImGuiWindow.h"

namespace VoxelEditor::gui
{
	class SceneViewport : public ImguiWindow
	{
	private:
		ImVec2 m_viewportSize;
		bool m_viewportFocused = true;
		TSharedPtr<Scene> m_scene;
		TSharedPtr<components::camera::EditorCameraController> m_camera;

		void drawRenderModes();
		void drawCameraModes();
	public:
		SceneViewport(const string& title, const TSharedPtr<Scene>& scene);
		~SceneViewport() override;
		
		INLINE bool wantCaptureKeyboard() const override;
		INLINE bool wantCaptureMouse() const override;
		bool hasToolbar() const override { return true; }
		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);
		FORCE_INLINE TSharedPtr<components::camera::EditorCameraController> getEditorCamera() const { return m_camera; }
		ImGuiWindowFlags flags() const override;

		INLINE void setClearColor(const glm::vec4& clearColor);

		void onBegin() override;
		void onImGuiRender() override;
		void onEnd() override;
		void onToolbar(const bool& hovered) override;
		void update(const Timestep& ts) override;
	};
}