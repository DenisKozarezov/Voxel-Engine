#include "EditorLayer.h"
#include <imgui.h>
#include <imgui_internal.h>

namespace VoxelEditor
{
	static bool show_demo_window = true;
	static bool show_performance;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{

	}

	const bool EditorLayer::onKeyPressed(const VoxelEngine::input::KeyPressedEvent& e) const
	{
		return false;
	}
	const bool EditorLayer::onMouseButtonPressed(const VoxelEngine::input::MouseButtonPressedEvent& e) const
	{
		return false;
	}

	void EditorLayer::drawMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("Open");
				ImGui::Separator();
				ImGui::MenuItem("Save");
				ImGui::MenuItem("Save As");
				ImGui::Separator();
				ImGui::MenuItem("Import");
				ImGui::MenuItem("Export");

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::MenuItem("Step Forward (Ctrl+D)");
				ImGui::MenuItem("Step Backwards (Ctrl+Z)");
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

			ImGui::EndMainMenuBar();
		}
	}
	void EditorLayer::drawBrushPanel()
	{
		//ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		ImVec2 regionSize = ImGui::GetContentRegionAvail();
		//ImGui::PopStyleVar();
		ImGui::End();
	}
	void EditorLayer::drawHierarchy()
	{
	}
	void EditorLayer::drawRenderPerformance()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoResize;
		if (show_performance && ImGui::Begin("Performance", &show_performance, flags))
		{
			ImGui::Text("Statistics:");

			ImGui::BeginColumns("statistics", 2);
			ImGui::Text("Draw Calls: %d", 0);
			ImGui::Text("Triangles: %d", 0);
			ImGui::Text("Vertices: %d", 0);
			ImGui::Text("Indices: %d", 0);
			ImGui::NextColumn();
			ImGui::Text("Voxels: %d", 0);
			ImGui::Text("SVO Traverse Time: %.3f ms", 0.0f);

			ImGui::EndColumns();

			ImGui::Separator();

			ImGuiIO& io = ImGui::GetIO();
			ImGui::Text("Rendering: %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
			ImGui::End();
		}
	}

	void EditorLayer::onAttach()
	{				  
		
	}				  
	void EditorLayer::onDetach()
	{				  
	}				  
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{				  
	}				  
	void EditorLayer::onImGuiRender()
	{
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);

		static float f = 0.0f;
		static int counter = 0;

		drawMenuBar();
		drawBrushPanel();
		drawRenderPerformance();

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}				  
	void EditorLayer::onEvent(VoxelEngine::input::Event& e)
	{
	}
}