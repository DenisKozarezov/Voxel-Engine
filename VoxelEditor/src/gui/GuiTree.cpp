#include "GuiTree.h"
#include "EditorConsole.h"

namespace VoxelEditor::gui
{
	GuiTree::GuiTree()
	{
		
	}
	void GuiTree::registerWindow(ImGuiWindow* window)
	{
        VOXEL_TRACE("Registering an ImGuiWindow with title '{0}'...", window->title());

#ifdef VOXEL_RELEASE
        const auto i = std::find(m_windows.begin(), m_windows.end(), window);
        if (i != m_windows.end())
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
                [](const ImGuiWindow* lhs, const ImGuiWindow* rhs) {
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
	void GuiTree::unregisterWindow(ImGuiWindow* window)
	{
        VOXEL_TRACE("Unregistering an ImGuiWindow with title '{0}'...", window->title());

        const auto i = std::remove(m_windows.begin(), m_windows.end(), window);
        if (i == m_windows.end())
            EditorConsole::error("Unable to unregister ImGuiWindow. There is no window with such title '{0}'", window->title());          
        else 
            m_windows.erase(i, m_windows.end());
	}
	void GuiTree::onImGuiRender()
	{
        bool windowWantsKeyboard = false;
        bool windowWantsMouse = false;

		for (ImGuiWindow* window : m_windows) 
		{
            window->onImGuiRender();
          /*  if (window->isVisible()) 
            {
                bool toolbarHovered = false;
                const bool isWindowVisible = window->begin();

                if (isWindowVisible) 
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
                const ImVec2 content_region_size{
                    contentRegionMax.x - contentRegionMin.x,
                    contentRegionMax.y - contentRegionMin.y
                };
                const ImVec2 toolbar_window_position
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
            }*/
		}
	}
}