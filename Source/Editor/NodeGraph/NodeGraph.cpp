#include "NodeGraph.h"
#include <imgui_internal.h>

namespace VoxelEditor::nodes
{
    NodeGraph::NodeGraph()
    {
        
    }

    NodeGraph::~NodeGraph()
    {
    }
    
    void NodeGraph::drawGrid(ImDrawList* drawList, const float& gridSize)
    {
        const ImU32 gridColor = ImColor(m_canvasProps.colors[ColCanvasLines]);
        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
        
        for (float x = fmodf(m_canvasProps.offset.x, gridSize); x < size.x;)
        {
            drawList->AddLine(ImVec2(x, 0.0f) + pos, ImVec2(x, size.y) + pos, gridColor);
            x += gridSize;
        }

        for (float y = fmodf(m_canvasProps.offset.y, gridSize); y < size.y;)
        {
            drawList->AddLine(ImVec2(0.0f, y) + pos, ImVec2(size.x, y) + pos, gridColor);
            y += gridSize;
        }
    }    

    void NodeGraph::onImGuiRender(ImDrawList* drawList)
    {
        ImGuiIO& io = ImGui::GetIO();
        const float gridSize = 64.0f * m_canvasProps.zoom;
        
        drawGrid(drawList, gridSize);
        
        ImGui::SetWindowFontScale(m_canvasProps.zoom);
    }
}
