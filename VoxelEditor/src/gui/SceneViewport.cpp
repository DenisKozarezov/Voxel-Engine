#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	void SceneViewport::drawRenderModes()
	{
		auto& settings = renderer::Renderer::getRenderSettings();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoMove;

		ImGui::BeginChild("##render_mode", { 300, 150 }, false, window_flags);
	
		ImGui::Text("Render Mode");

		static int current_item = 0;
		ImGui::Combo("##render_modes", &current_item, "Solid\0Wireframe\0Normals\0");

		ImGui::Checkbox("Show Editor Grid", &settings.showEditorGrid);
		ImGui::Checkbox("Show Octree", &settings.showOctree);
		ImGui::Checkbox("Show Normals Lines", &settings.showNormalsLines);

		ImGui::EndChild();

		settings.renderMode = static_cast<renderer::RenderMode>(current_item);
	}
	void SceneViewport::drawCameraModes()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoMove;
		ImRect basePos = ImGui::GetCurrentWindow()->WorkRect;
		ImVec2 pos = { basePos.Min.x + m_viewportSize.x - 400, basePos.Min.y };
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		
		ImGui::BeginChild("##cameraModes", { 400, 100 }, false, window_flags);
		ImGui::Text("Camera Mode");

		components::camera::ProjectionType projectionType = m_camera->projectionType();
		const char* modeStr = projectionTypeString(projectionType);
		if (ImGui::Button(modeStr))
		{
			int modeInt = (static_cast<int>(projectionType) + 1) % 2;
			m_camera->switchMode((components::camera::ProjectionType)modeInt);
		}

		ImGui::SameLine();
		static int cameraSpeed = 5;
		if (ImGui::SliderInt("Camera Speed", &cameraSpeed, 0, 30, "%d", ImGuiSliderFlags_AlwaysClamp))
			m_camera->setSpeed(cameraSpeed);

		ImGui::EndChild();
	}

	SceneViewport::SceneViewport(const string& title) : ImguiWindow(title)
	{
		glm::vec3 cameraPos = { 10.0f, 10.0f, 10.0f };

		m_camera = MakeUnique<components::camera::EditorCameraController>(cameraPos);
		subscribeEvent<input::MouseButtonPressedEvent>(BIND_CALLBACK(onMousePressed));
		subscribeEvent<input::MouseButtonReleasedEvent>(BIND_CALLBACK(onMouseReleased));
	}

	INLINE const bool SceneViewport::wantCaptureKeyboard() const
	{
		return ImGui::GetCurrentContext()->IO.WantCaptureKeyboard;
	}

	INLINE const bool SceneViewport::wantCaptureMouse() const
	{
		return ImGui::GetCurrentContext()->IO.WantCaptureMouse;
	}

	bool SceneViewport::onMousePressed(const input::MouseButtonPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			m_camera->setMouseDragging(true);
			break;
		}
		return true;
	}
	bool SceneViewport::onMouseReleased(const input::MouseButtonReleasedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			m_camera->setMouseDragging(false);
			break;
		}
		return true;
	}

	const ImGuiWindowFlags SceneViewport::flags() const
	{
		return ImGuiWindowFlags_NoCollapse;
	}

	INLINE void SceneViewport::setClearColor(const glm::vec4& clearColor)
	{
		renderer::RenderCommand::setClearColor(clearColor);
	}

	void SceneViewport::onBegin()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::SetNextWindowBgAlpha(0.0f);
	}

	void SceneViewport::onImGuiRender()
	{
		m_viewportSize = ImGui::GetContentRegionAvail();
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		if (m_viewportHovered && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			ImGui::SetWindowFocus();
		}

		const float aspectRatio = m_viewportSize.x / m_viewportSize.y;

		renderer::RenderCommand::setViewport(vMin.x, vMin.y, m_viewportSize.x, m_viewportSize.y);
		m_camera->setAspectRatio(aspectRatio);
		
		drawRenderModes();
		drawCameraModes();
	}

	void SceneViewport::onEnd()
	{
		ImGui::PopStyleVar();
	}

	void SceneViewport::update(const Timestep& ts)
	{	
		if (m_viewportFocused)
		{
			m_camera->update(ts);
		}
	}
}