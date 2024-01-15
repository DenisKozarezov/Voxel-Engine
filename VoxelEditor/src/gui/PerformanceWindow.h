#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
    class PerformanceWindow : public ImguiWindow
    {
    private:
        const renderer::RenderPerformanceStats* m_stats;
    public:
        PerformanceWindow(const string& title, const renderer::RenderPerformanceStats& stats);
        ~PerformanceWindow() override = default;

        ImGuiWindowFlags flags() const override;

        void onBegin() override;
        void onImGuiRender() override;
    };
}
