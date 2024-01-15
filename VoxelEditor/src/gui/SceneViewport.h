#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	class SceneViewport : public ImguiWindow
	{
	private:
		ImVec2 m_viewportSize;
		bool m_viewportFocused;
		bool m_viewportHovered;
		SharedRef<Scene> m_scene;
		UniqueRef<components::camera::EditorCameraController> m_camera;

		void drawRenderModes();
		void drawCameraModes();
	public:
		SceneViewport(const string& title, const SharedRef<Scene>& scene);
		~SceneViewport() override = default;
		
		INLINE bool wantCaptureKeyboard() const override;
		INLINE bool wantCaptureMouse() const override;
		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);
		ImGuiWindowFlags flags() const override;

		INLINE void setClearColor(const glm::vec4& clearColor);

		void onBegin() override;
		void onImGuiRender() override;
		void onEnd() override;
		void update(const Timestep& ts) override;
	};
}