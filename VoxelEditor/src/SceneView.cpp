#include "SceneView.h"
#include <imgui_impl_vulkan.h>

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
		//std::vector<VkDescriptorSet> m_Dset;

		//m_Dset.resize(3);
		//for (uint32_t i = 0; i < 3; i++)
		//	m_Dset[i] = ImGui_ImplVulkan_AddTexture(m_TextureSampler, m_ViewportImageViews[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
		ImGui::Begin("Viewport", 0, flags);

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		_viewportSize = { viewportPanelSize.x, viewportPanelSize.y };

		bool viewportFocused = ImGui::IsWindowFocused();
		bool viewportHovered = ImGui::IsWindowHovered();

		_camera->setAspectRatio(1920.0f / 1080.0f);
		
		//ImGui::Image(m_Dset[currentFrame], ImVec2{ viewportPanelSize.x, viewportPanelSize.y });

		drawRenderModes();
		
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