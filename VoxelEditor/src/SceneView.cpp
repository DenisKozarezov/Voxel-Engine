#include "SceneView.h"

namespace VoxelEditor
{
	void SceneView::drawRenderModes()
	{
		auto& settings = renderer::Renderer::getRenderSettings();

		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoMove;

		ImGui::BeginChild("##render_mode", { 300, 100 }, false, window_flags);
	
		ImGui::Text("Render Mode");

		static int current_item = 0;
		ImGui::Combo("##render_modes", &current_item, "Solid\0Wireframe\0Normals\0");

		ImGui::EndChild();

		settings.renderMode = static_cast<renderer::RenderMode>(current_item);
	}
	void SceneView::drawCameraModes()
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoMove;
		ImRect basePos = ImGui::GetCurrentWindow()->WorkRect;
		ImVec2 pos = { basePos.Min.x + m_viewportSize.x - 100, basePos.Min.y };
		ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
		
		ImGui::BeginChild("##cameraModes", { 100, 100 }, false, window_flags);
		ImGui::Text("Camera Mode");

		components::camera::ProjectionType projectionType = m_camera->projectionType();
		const char* modeStr = projectionTypeString(projectionType);
		if (ImGui::Button(modeStr))
		{
			int modeInt = (static_cast<int>(projectionType) + 1) % 2;
			m_camera->switchMode((components::camera::ProjectionType)modeInt);
		}
		ImGui::EndChild();
	}

	SceneView::SceneView()
	{
		glm::vec3 cameraPos = { 2.0f, 2.0f, 2.0f };

		m_camera = MakeUnique<components::camera::EditorCameraController>(cameraPos);
	}

	bool SceneView::onMousePressed(const input::MouseButtonPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			m_camera->setMouseDragging(true);
			break;
		}
		return true;
	}
	bool SceneView::onMouseReleased(const input::MouseButtonReleasedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			m_camera->setMouseDragging(false);
			break;
		}
		return true;
	}

	void SceneView::onImGuiRender()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", 0, flags);

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

		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SceneView::update(const Timestep& ts)
	{	
		if (m_viewportFocused)
		{
			m_camera->update(ts);
		}
	}
}