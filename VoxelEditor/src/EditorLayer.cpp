#include "EditorLayer.h"
#include "gui/EditorConsole.h"
#include "gui/PerformanceWindow.h"
#include "gui/UsefulToolsWindow.h"
#include "gui/PrimitivesPanel.h"

namespace VoxelEditor::gui
{
	static bool show_demo_window = false;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
		m_guiTree.registerViewport(MakeShared<SceneViewport>("Viewport"));
		m_guiTree.registerWindow(new EditorConsole("Console"));
		m_guiTree.registerWindow(new PrimitivesPanel("Add Primitives"));
		m_guiTree.registerWindow(new UsefulToolsWindow("Tools"));

		auto& stats = renderer::Renderer::getStats();
		m_guiTree.registerWindow(new PerformanceWindow("Performance", stats));
	}

	void EditorLayer::loadModel()
	{
		string filepath = utils::FileDialog::openFile(".obj");
		if (!filepath.empty()) 
		{
			if (m_loadedMesh)
			{
				EditorConsole::warn("Unregistering loaded mesh...");
				m_scene->unregisterMesh(m_loadedMesh);
			}

			EditorConsole::info("Attempting to load a resource '{0}'.", filepath);
			auto& mesh = assets::AssetsProvider::loadObjMesh(filepath);
			EditorConsole::warn("Loaded .obj file '{0}' [vertices: {1}; indices: {2}].", filepath, mesh->vertexCount(), mesh->indexCount());
			m_scene->registerMesh(mesh);
			m_loadedMesh = mesh;
		}
	}

	void EditorLayer::saveLayout()
	{
		string filepath = utils::FileDialog::saveFile(".ini");
		if (!filepath.empty())
		{
			Application::getImGuiLayer()->saveLayout(filepath);
			EditorConsole::info("ImGui layout saved.");
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
					if (ImGui::MenuItem("Save Layout"))
					{
						saveLayout();
					}
					ImGui::EndMenu();
				}
				ImGui::Separator();
				ImGui::MenuItem("Console");
				ImGui::MenuItem("Viewport");

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
	
	void EditorLayer::onAttach()
	{				  
		m_scene = MakeShared<Scene>();

		EditorConsole::info("Welcome to {0} {1}!", PROJECT_NAME, PROJECT_VERSION);
	}				  
	void EditorLayer::onDetach()
	{				
		
	}				  
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{
		m_guiTree.onUpdate(ts);
		
		const auto& viewport = m_guiTree.getViewport();
		m_scene->update(ts, *viewport->m_camera.get());
	}
	void EditorLayer::onFixedUpdate(const VoxelEngine::Timestep& ts)
	{
		
	}
	void EditorLayer::onImGuiRender()
	{
		static bool show = true;
		if (m_guiTree.showDockSpace(&show))
		{
			drawMenuBar();
			m_guiTree.onImGuiRender();		
			ImGui::End();
		}
		
		if (show_demo_window)
			ImGui::ShowDemoWindow(&show_demo_window);
	}			  
	void EditorLayer::onEvent(input::Event& e)
	{
		switch (e.eventType())
		{
			case input::EventType::MouseButtonPressed: case input::EventType::MouseButtonReleased:
				m_guiTree.getViewport()->sendEvent(e);
			break;
		}
	}
}
