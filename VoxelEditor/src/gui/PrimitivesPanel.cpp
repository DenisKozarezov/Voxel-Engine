#include "PrimitivesPanel.h"

namespace VoxelEditor::gui
{
	PrimitivesPanel::PrimitivesPanel(const string& title) : ImguiWindow(title)
	{

	}
	
    void PrimitivesPanel::onImGuiRender()
	{
        static ImGuiTableFlags flags1 = ImGuiTableFlags_BordersV;
        ImGui::SeparatorText("Mesh");
        if (ImGui::BeginTable("table_padding", 1, flags1))
        {
            ImGui::TableNextRow();
            for (int column = 0; column < 1; column++)
            {
                ImGui::TableSetColumnIndex(column);
                ImGui::Button("Sphere", ImVec2(-FLT_MIN, 0.0f));
                ImGui::Button("Torus", ImVec2(-FLT_MIN, 0.0f));
                ImGui::Button("Plane", ImVec2(-FLT_MIN, 0.0f));
                ImGui::Button("Ellipsoid", ImVec2(-FLT_MIN, 0.0f));
            }
            ImGui::EndTable();
        }
	}
}