#include "SceneView.h"

namespace VoxelEditor
{
	void SceneView::drawRenderModes()
	{
		static int e;
		auto& settings = renderer::Renderer::getRenderSettings();

		ImGui::BeginChild("##render_modes", { 200, 100 });
		ImGui::RadioButton("Solid", &e, 0);
		ImGui::RadioButton("Wireframe", &e, 1);
		ImGui::Separator();
		ImGui::Checkbox("Show Normals", &settings.showNormals);
		ImGui::EndChild();

		settings.renderMode = static_cast<renderer::RenderMode>(e);
	}
	SceneView::SceneView()
	{
		glm::vec3 cameraPos = { 2.0f, 2.0f, 2.0f };

		_camera = MakeUnique<components::camera::FirstPersonCamera>(cameraPos);
	}
	bool SceneView::onMousePressed(const input::MouseButtonPressedEvent& e)
	{
		if (!m_viewportFocused)
			return false;

		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(true);
			break;
		}
		return true;
	}
	bool SceneView::onMouseReleased(const input::MouseButtonReleasedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(false);
			break;
		}
		return true;
	}
	bool SceneView::onMouseMoved(const input::MouseMovedEvent& e)
	{
		const float x = e.getX();
		const float y = e.getY();

		if (_mouseState == input::MouseDraggingState::DragBegin)
		{
			_lastMouseX = x;
			_lastMouseY = y;
			_mouseState = input::MouseDraggingState::Dragging;
		}

		if (_mouseState == input::MouseDraggingState::Dragging)
		{
			mouseMove(x - _lastMouseX, _lastMouseY - y);
			_lastMouseX = x;
			_lastMouseY = y;
		}
		return true;
	}
	void SceneView::render()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground;
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Viewport", 0, flags);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		vMin.x += ImGui::GetWindowPos().x;
		vMin.y += ImGui::GetWindowPos().y;

		m_viewportFocused = ImGui::IsWindowFocused();
		m_viewportHovered = ImGui::IsWindowHovered();
		if (m_viewportHovered && ImGui::IsMouseDown(1))
		{
			ImGui::SetWindowFocus();
		}

		renderer::RenderCommand::setViewport(vMin.x, vMin.y, viewportPanelSize.x, viewportPanelSize.y);
		_camera->setAspectRatio(viewportPanelSize.x / viewportPanelSize.y);

		drawRenderModes();
		
		ImGui::PopStyleVar();
		ImGui::End();
	}

	void SceneView::setMouseDragging(const bool& isDragging)
	{
		_mouseState = isDragging ? input::MouseDraggingState::DragBegin : input::MouseDraggingState::None;
	}
	void SceneView::moveCamera(const components::camera::CameraMovement& direction, const float& deltaTime)
	{
		_camera->processKeyboard(direction, deltaTime);
	}
	void SceneView::mouseMove(const float& x, const float& y)
	{
		_camera->processMouse(x, y);
	}
}