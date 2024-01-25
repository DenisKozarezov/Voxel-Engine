#include "UsefulToolsWindow.h"
#include "GuiTree.h"
#include "../NodeGraph/NodeEditor.h"

namespace VoxelEditor::gui
{
    UsefulToolsWindow::UsefulToolsWindow(const string& title, GuiTree* guiTree) : ImguiWindow(title),
        m_guiTree(guiTree)
    {
    }

    void UsefulToolsWindow::onImGuiRender()
    {
        if (ImGui::Button("Node Editor"))
        {
            m_guiTree->registerWindow(new nodes::NodeEditor("Node Editor"));
        }
    }
}
