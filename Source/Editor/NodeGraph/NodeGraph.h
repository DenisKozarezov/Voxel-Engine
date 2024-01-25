#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "Nodes/NodeBase.h"

namespace VoxelEditor::nodes
{
    enum CanvasStyleColor
    {
        ColCanvasLines,
        ColNodeBg,
        ColNodeActiveBg,
        ColNodeBorder,
        ColConnection,
        ColConnectionActive,
        ColSelectBg,
        ColSelectBorder,
        ColMax
    };
    
    struct CanvasProperties
    {
    public:
        float zoom = 1.0f;
        ImVec2 offset{};
        ImColor colors[ColMax];

        CanvasProperties()
        {
            const auto& style = ImGui::GetStyle();
            colors[ColCanvasLines] = style.Colors[ImGuiCol_Separator];
            colors[ColNodeBg] = style.Colors[ImGuiCol_WindowBg];
            colors[ColNodeActiveBg] = style.Colors[ImGuiCol_FrameBgActive];
            colors[ColNodeBorder] = style.Colors[ImGuiCol_Border];
            colors[ColConnection] = style.Colors[ImGuiCol_PlotLines];
            colors[ColConnectionActive] = style.Colors[ImGuiCol_PlotLinesHovered];
            colors[ColSelectBg] = style.Colors[ImGuiCol_FrameBgActive];
            colors[ColSelectBg].Value.w = 0.25f;
            colors[ColSelectBorder] = style.Colors[ImGuiCol_Border];
        }
    };
    
    class NodeGraph
    {
    private:
        CanvasProperties m_canvasProps{};
        TUniquePtr<NodeBase> m_rootNode = nullptr;

        void drawGrid(ImDrawList* drawList, const float& gridSize);
    public:
        NodeGraph();
        ~NodeGraph();
        
        FORCE_INLINE const ImVec2& getOffset() const { return m_canvasProps.offset; }
        FORCE_INLINE const float& getZoom() const { return m_canvasProps.zoom; }
        FORCE_INLINE const ImColor& getStyleColor(CanvasStyleColor style) const { return m_canvasProps.colors[style]; }
        
        void setOffset(const ImVec2& offset) { m_canvasProps.offset = offset; }
        void setZoom(const float& zoom) { m_canvasProps.zoom = zoom; }
        
        void onImGuiRender(ImDrawList* drawList);
    };
}
