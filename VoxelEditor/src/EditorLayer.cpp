#include "EditorLayer.h"

namespace VoxelEditor
{
	static bool show_demo_window = false;
	static bool show_performance;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
		
	}

	void EditorLayer::loadModel()
	{
		string filepath = utils::FileDialog::openFile(".obj");
		if (!filepath.empty()) 
		{
			auto& mesh = assets::AssetsProvider::loadObjMesh(filepath);

			input::MeshLoadedEvent e = input::MeshLoadedEvent(mesh);
			input::EventDispatcher::dispatchEvent(e);	
		}
	}

	void EditorLayer::drawMenuBar()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "Ctrl+O"))
				{ 
					loadModel();
				}

				ImGui::Separator();
				if (ImGui::MenuItem("Save", "Ctrl+S"))
				{
					utils::FileDialog::saveFile(".obj");
				}
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
				ImGui::MenuItem("Console");
				ImGui::MenuItem("Viewport");

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
	void EditorLayer::drawRenderPerformance()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;
		if (show_performance && ImGui::Begin("Performance", &show_performance, flags))
		{
			auto stats = renderer::Renderer::getStats();

			if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::BeginColumns("##statistics", 2);
				ImGui::Text("Draw Calls: %d", stats.frameStats.drawCalls);
				ImGui::Text("Triangles: %d", stats.frameStats.triangles);
				ImGui::Text("Vertices: %d", stats.frameStats.vertices);
				ImGui::Text("Indices: %d", stats.frameStats.indices);
				ImGui::Text("Instances: %d", stats.frameStats.instances);

				ImGui::Text("Batches: %d", 0);
				ImGui::EndColumns();

				ImGui::Separator();

				ImGui::Text("Rendering: %.3f ms/frame (%i FPS)", stats.deltaTime * 1000.0f, stats.fps);
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
		input::EventDispatcher::registerEvent<input::MouseButtonPressedEvent>(BIND_MEMBER_CALLBACK(&m_sceneView, SceneView::onMousePressed));
		input::EventDispatcher::registerEvent<input::MouseButtonReleasedEvent>(BIND_MEMBER_CALLBACK(&m_sceneView, SceneView::onMouseReleased));

		m_scene = MakeShared<Scene>();

		EditorConsole::info("Welcome to {0} {1}!", PROJECT_NAME, PROJECT_VERSION);
	}				  
	void EditorLayer::onDetach()
	{				
		
	}				  
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{
		m_sceneView.update(ts);
		m_scene->update(ts, *m_sceneView.m_camera.get());
	}
	void EditorLayer::onFixedUpdate(const VoxelEngine::Timestep& ts)
	{
		
	}
	void EditorLayer::onImGuiRender()
	{
		static bool dockspaceOpen = true;
		static bool opt_fullscreen_persistant = true;
		static bool opt_fullscreen = true;

		// Full-size
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
		ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspaceOpen, flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 100.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minWinSizeX;

		drawMenuBar();
		m_sceneView.onImGuiRender();

		drawRenderPerformance();

		ImGui::Begin("Palette");
		ImGui::End();

		ImGui::Begin("Inspector");
		ImGui::End();

		m_console.render();

		ImGui::End();

		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}				  
	void EditorLayer::onEvent(input::Event& e)
	{
		input::EventDispatcher::dispatchEvent(e, std::launch::async);
	}
}