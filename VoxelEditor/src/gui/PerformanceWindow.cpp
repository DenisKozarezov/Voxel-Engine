#include "PerformanceWindow.h"

namespace VoxelEditor::gui
{
    PerformanceWindow::PerformanceWindow(const string& title, const renderer::RenderPerformanceStats& stats) : ImguiWindow(title)
    {
        m_stats = &stats;
    }

    ImGuiWindowFlags PerformanceWindow::flags() const
    {
        return ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
    }

    void PerformanceWindow::onBegin()
    {
        ImGui::SetNextWindowBgAlpha(1.0f);
    }

    void PerformanceWindow::onImGuiRender()
    {
        if (ImGui::CollapsingHeader("Statistics", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::BeginColumns("##statistics", 2);
            ImGui::Text("Draw Calls: %d", m_stats->frameStats.drawCalls);
            ImGui::Text("Triangles: %d", m_stats->frameStats.triangles);
            ImGui::Text("Vertices: %d", m_stats->frameStats.vertices);
            ImGui::Text("Indices: %d", m_stats->frameStats.indices);
            ImGui::Text("Instances: %d", m_stats->frameStats.instances);
            ImGui::Text("Batches: %d", 0);
            
            ImGui::NextColumn();
            for (int i = 0; i < m_stats->shaderStats.performanceStrings.size(); ++i)
            {
                ImGui::Text("%s: %d", m_stats->shaderStats.performanceStrings[i], m_stats->shaderStats.performanceStats[i]);
            }
            
            ImGui::EndColumns();

            ImGui::Separator();

            ImGui::Text("Rendering: %.3f ms/frame (%i FPS)", m_stats->deltaTime * 1000.0f, m_stats->fps);
        }

        if (ImGui::CollapsingHeader("Memory"))
        {
            ImGui::Text("Textures Memory: %d MB", 0);
            ImGui::Text("Voxels Memory: %d MB", 0);
				
            ImGui::BeginGroup();
            ImGui::AlignTextToFramePadding();
            ImGui::Text("Memory Usage:");
            ImGui::SameLine();
            ImGui::ProgressBar(0.6f, ImVec2(0,0), "0 MB / 1000 MB");
            ImGui::EndGroup();
        }
    }
}