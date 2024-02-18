#include "CanvasMoveManipulator.h"
#include <NodeGraph/Editor/NodeEditor.h>

namespace VoxelEditor::nodes
{
    CanvasMoveManipulator::CanvasMoveManipulator(const NodeEditor* nodeEditor)
        : ICanvasManipulator(nodeEditor)
    { }
    
    void CanvasMoveManipulator::manipulateGraph(NodeGraph& graph)
    {
        if (!m_editor)
            return;
        
        ImGuiIO& io = ImGui::GetIO();        
        if (!m_editor->isMouseDown(ImGuiMouseButton_Left) && m_editor->isHovered())
        {
            const ImVec2 currentOffset = graph.getOffset();
            
            if (m_editor->isMouseDragging(ImGuiMouseButton_Right))
                graph.setOffset(currentOffset + ImVec2(io.MouseDelta));
        }
    }
}
