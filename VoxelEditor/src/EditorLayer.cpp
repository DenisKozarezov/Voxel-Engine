#include "EditorLayer.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace VoxelEditor
{
	static bool show_demo_window = true;
	static bool show_performance;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
		_mouseState = input::MouseDraggingState::None;
	}

	bool EditorLayer::onKeyboardPressed(const input::KeyPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::W:
			moveCamera(components::camera::CameraMovement::Forward);
			break;
		case input::S:
			moveCamera(components::camera::CameraMovement::Backward);
			break;
		case input::A:
			moveCamera(components::camera::CameraMovement::Left);
			break;
		case VoxelEngine::input::D:
			moveCamera(components::camera::CameraMovement::Right);
			break;
		}
		return true;
	}
	bool EditorLayer::onMousePressed(const input::MouseButtonPressedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(true);
			break;
		}
		return true;
	}
	bool EditorLayer::onMouseReleased(const input::MouseButtonReleasedEvent& e)
	{
		switch (e.getKeyCode())
		{
		case input::ButtonRight:
			setMouseDragging(false);
			break;
		}
		return true;
	}
	bool EditorLayer::onMouseMoved(const input::MouseMovedEvent& e)
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

	void EditorLayer::setMouseDragging(const bool& isDragging)
	{
		_mouseState = isDragging ? input::MouseDraggingState::DragBegin : input::MouseDraggingState::None;
	}
	void EditorLayer::moveCamera(const components::camera::CameraMovement& direction)
	{		
		_camera.processKeyboard(direction, _fixedDeltaTime);
	}
	void EditorLayer::mouseMove(const float& x, const float& y)
	{
		_camera.processMouse(x, y);
	}

	void EditorLayer::drawMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Open", "Ctrl+O");
				ImGui::Separator();
				ImGui::MenuItem("Save", "Ctrl+S");
				ImGui::MenuItem("Save As...");
				ImGui::Separator();
				ImGui::MenuItem("Import");
				ImGui::MenuItem("Export");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Step Forward", "Ctrl+D");
				ImGui::MenuItem("Step Backwards", "Ctrl+Z");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Profiler");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::BeginMenu("Layout"))
				{
					ImGui::EndMenu();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Performance"))
				{
					show_performance = true;
				}

				ImGui::MenuItem("Editor Settings");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				ImGui::MenuItem("About");
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
	void EditorLayer::drawRenderModes()
	{
		static int e;
		auto& settings = VoxelEngine::renderer::Renderer::getRenderSettings();

		ImGui::BeginChild("##render_modes", { 200, 100 });
		ImGui::RadioButton("Solid", &e, 0);
		ImGui::RadioButton("Wireframe", &e, 1);
		ImGui::Separator();
		ImGui::Checkbox("Show Normals", &settings.showNormals);
		ImGui::EndChild();

		settings.renderMode = static_cast<VoxelEngine::renderer::RenderMode>(e);
	}
	void EditorLayer::drawRenderPerformance()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		if (show_performance && ImGui::Begin("Performance", &show_performance, flags))
		{
			auto stats = VoxelEngine::renderer::Renderer::getStats();

			if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::BeginColumns("##statistics", 2);
				ImGui::Text("Draw Calls: %d", stats.drawCalls);
				ImGui::Text("Triangles: %d", stats.triangles);
				ImGui::Text("Vertices: %d", stats.vertices);
				ImGui::Text("Indices: %d", stats.indices);
				ImGui::NextColumn();

				for (int i = 0; i < 6; ++i)
				{
					ImGui::Text("%s: %d", stats.pipelineStatNames[i].c_str(), stats.pipelineStats[i]);
				}

				ImGui::Text("Batches: %d", 0);
				ImGui::EndColumns();

				ImGui::Separator();

				ImGui::Text("Rendering: %.3f ms/frame (%.1f FPS)", stats.deltaTime * 1000.0f, stats.fps);
			}

			if (ImGui::CollapsingHeader("Memory"))
			{
				ImGui::Text("Textures Memory: %d MB", 0);
				ImGui::Text("Voxels Memory: %d MB", 0);
				
				ImGui::BeginGroup();
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Memory Usage:");
				ImGui::SameLine();
				ImGui::ProgressBar(0.6f, ImVec2(0,0), "0 MB / 1000 MB");
				ImGui::EndGroup();
			}

			ImGui::End();
		}
	}

	void EditorLayer::onAttach()
	{				  
		_dispatcher.registerEvent<input::KeyPressedEvent>(BIND_CALLBACK(EditorLayer::onKeyboardPressed));
		_dispatcher.registerEvent<input::MouseButtonPressedEvent>(BIND_CALLBACK(EditorLayer::onMousePressed));
		_dispatcher.registerEvent<input::MouseButtonReleasedEvent>(BIND_CALLBACK(EditorLayer::onMouseReleased));
		_dispatcher.registerEvent<input::MouseMovedEvent>(BIND_CALLBACK(EditorLayer::onMouseMoved));
	
		renderer::Renderer::setCamera(&_camera);
		renderer::Renderer::setScene(new Scene());
	}				  
	void EditorLayer::onDetach()
	{				
  
	}				  
	void EditorLayer::onFixedUpdate(const VoxelEngine::Timestep& ts)
	{
		_fixedDeltaTime = ts;
	}
	void EditorLayer::onImGuiRender()
	{
		static bool opt_fullscreen = true;

		// Full-size
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar;
		if (opt_fullscreen)
		{
			flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
			flags |= ImGuiWindowFlags_NoBackground;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			ImGui::PopStyleVar(2);
		}

		if (ImGui::Begin("##root", 0, flags))
		{
			drawRenderPerformance();
			drawMenuBar();
			drawRenderModes();

			ImGui::End();
		}

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}				  
	void EditorLayer::onEvent(VoxelEngine::input::Event& e)
	{
		_dispatcher.dispatchEvent(e, std::launch::async);
	}
}