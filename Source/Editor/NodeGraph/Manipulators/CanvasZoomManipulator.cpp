#include "CanvasZoomManipulator.h"
#include "../NodeGraph.h"

namespace VoxelEditor::nodes
{
    CanvasZoomManipulator::CanvasZoomManipulator(const NodeEditor* nodeEditor) 
        : ICanvasManipulator(nodeEditor)
    { }

    void CanvasZoomManipulator::manipulateGraph(NodeGraph& graph)
    {
        ImGuiIO& io = ImGui::GetIO();        
        if (io.KeyCtrl && io.MouseWheel != 0.0f)
        {
            const float currentZoom = graph.getZoom();
            const float newZoom = ImClamp(currentZoom + io.MouseWheel * currentZoom / 16.f, 0.3f, 3.0f);
            const ImVec2 currentOffset = graph.getOffset();
            graph.setZoom(newZoom);
            graph.setOffset(currentOffset + ImGui::GetMouseDragDelta());
        }
    }
}
