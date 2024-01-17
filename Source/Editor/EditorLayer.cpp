#include "EditorLayer.h"
#include "gui/EditorConsole.h"
#include "gui/PerformanceWindow.h"
#include "gui/UsefulToolsWindow.h"
#include "gui/PrimitivesPanel.h"
#include <Core/version.h>

namespace VoxelEditor::gui
{
	static bool show_demo_window = false;

	EditorLayer::EditorLayer() : Layer("EditorLayer")
	{
		m_scene = MakeShared<Scene>();
		
		m_guiTree.registerViewport(new SceneViewport("Viewport", m_scene));
		m_guiTree.registerWindow(new EditorConsole("Console"));
		m_guiTree.registerWindow(new PrimitivesPanel("Add Primitives"));
		m_guiTree.registerWindow(new UsefulToolsWindow("Tools", &m_guiTree));
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
				if (ImGui::MenuItem("Console"))
				{
					m_guiTree.registerWindow(new EditorConsole("Console"));
				}
				
				if (ImGui::MenuItem("Viewport"))
				{
					m_guiTree.registerViewport(new SceneViewport("Viewport", m_scene));
				}
				
				ImGui::Separator();
				if (ImGui::MenuItem("Performance"))
				{
					auto& stats = renderer::Renderer::getStats();
					m_guiTree.registerWindow(new PerformanceWindow("Performance", stats));
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
	
	void EditorLayer::onAttach()
	{				  
		EditorConsole::info("Welcome to {0} {1}!", PROJECT_NAME, PROJECT_VERSION);
	}				  
	void EditorLayer::onDetach()
	{
		
	}
	void EditorLayer::onUpdate(const VoxelEngine::Timestep& ts)
	{
		renderer::Renderer::resetStats();

		if (const SceneViewport* viewport = m_guiTree.getViewport())
			renderer::Renderer::preRender(*viewport->getEditorCamera().get());
		else
			renderer::Renderer::preRender();
		
		m_guiTree.onUpdate(ts);

		renderer::Renderer::render();

		renderer::Renderer::postRender();

		renderer::Renderer::flushStats();
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
			m_guiTree.sendEvent(e);
			break;
		}
	}
}
