#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>

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
        ImVec2 m_offset{};
        ImColor colors[ColMax];

        CanvasProperties()
        {
            const auto& imgui_style = ImGui::GetStyle();
            colors[ColCanvasLines] = imgui_style.Colors[ImGuiCol_Separator];
            colors[ColNodeBg] = imgui_style.Colors[ImGuiCol_WindowBg];
            colors[ColNodeActiveBg] = imgui_style.Colors[ImGuiCol_FrameBgActive];
            colors[ColNodeBorder] = imgui_style.Colors[ImGuiCol_Border];
            colors[ColConnection] = imgui_style.Colors[ImGuiCol_PlotLines];
            colors[ColConnectionActive] = imgui_style.Colors[ImGuiCol_PlotLinesHovered];
            colors[ColSelectBg] = imgui_style.Colors[ImGuiCol_FrameBgActive];
            colors[ColSelectBg].Value.w = 0.25f;
            colors[ColSelectBorder] = imgui_style.Colors[ImGuiCol_Border];
        }
    };
    
    class NodeGraph
    {
    private:
        CanvasProperties m_canvasProps{};
    public:
        void onImGuiRender(ImDrawList* drawList);
    };
}
