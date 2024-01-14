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

        const ImGuiWindowFlags& flags() const override;
        
        void onImGuiRender() override;
    };
}
