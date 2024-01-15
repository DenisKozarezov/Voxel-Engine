#include "GuiTree.h"
#include "EditorConsole.h"

namespace VoxelEditor::gui
{
	GuiTree::GuiTree()
	{
		
	}
	void GuiTree::registerWindow(ImguiWindow* window)
	{	    
		VOXEL_TRACE("Registering an ImGuiWindow with title '{0}'...", window->title());

#ifdef VOXEL_RELEASE
		const auto it = std::find(m_windows.begin(), m_windows.end(), window);
		if (it != m_windows.end())
		{
			EditorConsole::error("ImGuiWindow with title '{0}' already registered.", window->title());
		}
		else
#endif
		{            
			m_windows.emplace_back(window);
			std::sort(
				m_windows.begin(),
				m_windows.end(),
				[](const ImguiWindow* lhs, const ImguiWindow* rhs) {
					return lhs->title() < rhs->title();
				}
			);
		}
	}
	void GuiTree::registerViewport(SceneViewport* viewport)
	{
		m_viewport = viewport;
		registerWindow(viewport);
	}

	void GuiTree::unregisterWindow(const ImguiWindow* window)
	{
		if (window == m_viewport)
			m_viewport = nullptr;
		
		const auto it = std::find(m_windows.begin(), m_windows.end(), window);
		if (it == m_windows.end())
		{
			EditorConsole::error("There is no ImGuiWindow with such title '{0}'.", window->title());
		}
		else
		{
			VOXEL_TRACE("Unregistering an ImGuiWindow with title '{0}'...", window->title());
			m_windows.erase(it);
			delete window;
		}
	}

	void GuiTree::unregisterWindow(std::vector<ImguiWindow*>::iterator it)
	{
		if (*it == m_viewport)
			m_viewport = nullptr;
		
		if (it == m_windows.end())
		{
			EditorConsole::error("Unable to unregister an ImGuiWindow: invalid iterator.");
		}
		else
		{
			VOXEL_TRACE("Unregistering an ImGuiWindow with title '{0}'...", (*it)->title());
			const ImguiWindow* window = *it;
			m_windows.erase(it);
			delete window;
		}
	}
	
	bool GuiTree::showDockSpace(bool* p_open) const
	{
		static bool opt_fullscreen_persistant = true;
		bool opt_fullscreen = opt_fullscreen_persistant;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
 
		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
 
		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background 
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;
 
		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		bool begin = ImGui::Begin("###DockSpace", p_open, window_flags);
		ImGui::PopStyleVar();
 
		if (opt_fullscreen)
			ImGui::PopStyleVar(2);
 
		// DockSpace
		ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
	    
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		float minWinSizeX = style.WindowMinSize.x;
		style.WindowMinSize.x = 100.0f;

		style.WindowMinSize.x = minWinSizeX;
	    
		return begin;
	}

	void GuiTree::onImGuiRender()
	{
		bool windowWantsKeyboard = false;
		bool windowWantsMouse = false;

		for (ImguiWindow* window : m_windows)
		{
			if (!window->isVisible())
			{
				input::WindowCloseEvent e;
				window->sendEvent(e);
				unregisterWindow(window);
				continue;
			}

			bool toolbarHovered = false;

			if (window->begin()) 
			{
				const ImVec2 beforeCursorPos = ImGui::GetCursorPos();
				window->onImGuiRender();
				if (window->hasToolbar())
				{
					ImGui::SetCursorPos(beforeCursorPos);
					window->onToolbar(toolbarHovered);
				}
			}
                
			const auto windowPos = ImGui::GetWindowPos();
			const auto windowSize = ImGui::GetWindowSize();
			const auto contentRegionMin = ImGui::GetWindowContentRegionMin();
			const auto contentRegionMax = ImGui::GetWindowContentRegionMin();
			const ImVec2 contentRegionSize{
				contentRegionMax.x - contentRegionMin.x,
				contentRegionMax.y - contentRegionMin.y
			};
			const ImVec2 toolbarWindowPosition
			{
				windowPos.x + contentRegionMin.x,
				windowPos.y + contentRegionMin.y
			};
			const bool windowHovered = ImGui::IsWindowHovered();
			if (!toolbarHovered && windowHovered) 
			{
				windowWantsKeyboard |= window->wantCaptureKeyboard();
				windowWantsMouse |= window->wantCaptureMouse();
			}

			window->end();
		}
	}

	void GuiTree::onUpdate(const Timestep& ts)
	{
		for (ImguiWindow* window : m_windows)
		{
			window->update(ts);
		}
	}

	void GuiTree::sendEvent(input::Event& e)
	{
		for (ImguiWindow* window : m_windows)
		{
			window->sendEvent(e);
		}
	}

	GuiTree::~GuiTree() noexcept
	{
		while (!m_windows.empty())
		{
			auto it = m_windows.begin();
			unregisterWindow(it);
		}
	}
}
