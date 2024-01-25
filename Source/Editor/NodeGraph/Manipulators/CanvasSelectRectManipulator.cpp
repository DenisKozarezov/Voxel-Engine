#include "CanvasSelectRectManipulator.h"
#include "../NodeEditor.h"

namespace VoxelEditor::nodes
{
    CanvasSelectRectManipulator::CanvasSelectRectManipulator(const NodeEditor* nodeEditor)
        : ICanvasManipulator(nodeEditor)
    {
    }

    void CanvasSelectRectManipulator::manipulateGraph(NodeGraph& graph)
    {
        if (!m_editor)
            return;

        ImDrawList* drawList = ImGui::GetWindowDrawList();

        m_mouseDown = m_editor->isMouseDown(ImGuiMouseButton_Left);
        if (!m_mouseDown)
            m_startSelectPos = ImGui::GetMousePos();
            
        if (m_mouseDown)
        {
            const ImVec2 mousePos = ImGui::GetMousePos();
            drawList->AddRectFilled(m_startSelectPos, mousePos, graph.getStyleColor(ColSelectBg));
            drawList->AddRect(m_startSelectPos, mousePos, graph.getStyleColor(ColSelectBorder));
        }
        else
        {
            ImGui::ClearActiveID();
        }
    }
}
