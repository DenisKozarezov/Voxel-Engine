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
	void EditorLayer::drawHierarchy()
	{
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
				ImGui::Text("Draw Calls: %d", 0);
				ImGui::Text("Triangles: %d", 0);
				ImGui::Text("Vertices: %d", 0);
				ImGui::Text("Indices: %d", 0);
				ImGui::NextColumn();
				ImGui::Text("Batches: %d", 0);
				ImGui::EndColumns();

				ImGui::Separator();

				ImGui::Text("Rendering: %.3f ms/frame (%.1f FPS)", stats.deltaTime, stats.fps);
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
		
	}				  
	void EditorLayer::onDetach()
	{				  
	}				  
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{				  
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

			ImGui::End();
		}

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}				  
	void EditorLayer::onEvent(VoxelEngine::input::Event& e)
	{
	}
}