#include "NodeDrawer.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include <NodeGraph/Runtime/NodeGraph.h>

namespace VoxelEditor::nodes
{
    bool NodeDrawer::beginNode(ImDrawList* drawList)
    {
        // Create 2 UI layers (one for nodes). Just like layers in Photoshop or Unity. 
        drawList->ChannelsSplit(2);

        // Node position relative to the window.
        ImGui::SetCursorScreenPos(ImGui::GetWindowPos() + m_node->position() * m_canvasProps->zoom + m_canvasProps->offset);

        // Node size
        ImGuiStyle& style = ImGui::GetStyle();
        style.FrameRounding = 1.0f;

        ImGui::PushID(m_node.get());
        
        ImGui::BeginGroup();

        drawList->ChannelsSetCurrent(1); // We are now in Nodes layer
        
        return true;
    }

    void NodeDrawer::endNode(ImDrawList* drawList)
    {
        ImGui::EndGroup();

        const ImGuiStyle& style = ImGui::GetStyle();
        const ImRect nodeRect{
            ImGui::GetItemRectMin() - style.ItemInnerSpacing * m_canvasProps->zoom,
            ImGui::GetItemRectMax() + style.ItemInnerSpacing * m_canvasProps->zoom
        };
        
        drawList->ChannelsSetCurrent(0); // Don't forget to get back to default UI layer
        
        const ImColor nodeColor = m_canvasProps->colors[ColNodeBg];
        drawList->AddRectFilled(nodeRect.Min, nodeRect.Max, nodeColor, style.FrameRounding);
        drawList->AddRect(nodeRect.Min, nodeRect.Max, m_canvasProps->colors[ColNodeBorder], style.FrameRounding);
        ImGui::ItemSize(nodeRect.GetSize());
        
        drawList->ChannelsMerge();
        
        ImGui::PopID();
    }
}
