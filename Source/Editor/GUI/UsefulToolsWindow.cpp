#include "UsefulToolsWindow.h"

namespace VoxelEditor::gui
{
    UsefulToolsWindow::UsefulToolsWindow(const string& title) : ImguiWindow(title)
    {
    }

    void UsefulToolsWindow::onImGuiRender()
    {
        if (ImGui::Button("Node Editor"))
        {
            
        }
    }
}
