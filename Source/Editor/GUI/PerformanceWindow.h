#pragma once
#include "ImGuiWindow.h"

namespace VoxelEditor::gui
{
    class PerformanceWindow : public ImguiWindow
    {
    private:
        const RenderPerformanceStats* m_stats;
    public:
        PerformanceWindow(const string& title, const RenderPerformanceStats& stats);
        ~PerformanceWindow() override = default;

        ImGuiWindowFlags flags() const override;

        void onBegin() override;
        void onImGuiRender() override;
    };
}
