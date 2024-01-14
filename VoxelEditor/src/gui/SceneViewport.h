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

		void drawRenderModes();
		void drawCameraModes();
	public:
		UniqueRef<components::camera::EditorCameraController> m_camera;

		SceneViewport(const string& title);
		~SceneViewport() override = default;
		
		INLINE const bool wantCaptureKeyboard() const override;
		INLINE const bool wantCaptureMouse() const override;
		bool onMousePressed(const input::MouseButtonPressedEvent& e);
		bool onMouseReleased(const input::MouseButtonReleasedEvent& e);
		const ImGuiWindowFlags flags() const override;

		INLINE void setClearColor(const glm::vec4& clearColor);

		void onBegin() override;
		void onImGuiRender() override;
		void onEnd() override;
		void update(const Timestep& ts) override;
	};
}