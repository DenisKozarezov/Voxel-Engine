#include "NodeGraph.h"
#include <cmath>

namespace VoxelEditor::nodes
{
    void NodeGraph::onImGuiRender(ImDrawList* drawList)
    {
        const float grid = 64.0f * m_canvasProps.zoom;

        const ImVec2 pos = ImGui::GetWindowPos();
        const ImVec2 size = ImGui::GetWindowSize();
        const ImU32 grid_color = ImColor(m_canvasProps.colors[ColCanvasLines]);
        
        for (float x = fmodf(m_canvasProps.m_offset.x, grid); x < size.x;)
        {
            drawList->AddLine(ImVec2(x, 0.0f) + pos, ImVec2(x, size.y) + pos, grid_color);
            x += grid;
        }

        for (float y = fmodf(m_canvasProps.m_offset.y, grid); y < size.y;)
        {
            drawList->AddLine(ImVec2(0.0f, y) + pos, ImVec2(size.x, y) + pos, grid_color);
            y += grid;
        }

        ImGui::SetWindowFontScale(m_canvasProps.zoom);
    }
}
