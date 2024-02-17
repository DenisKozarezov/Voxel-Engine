#include "PrimitivesPanel.h"
#include <Engine/Tests/VModelsTests.h>

namespace VoxelEditor::gui
{
	void PrimitivesPanel::onHomotopyApply(const double& homotopyFactor)
	{
		if (const auto& scene = m_scene.lock())
		{
			const auto& mesh = CreateTestHomotopy({200, 200, 200}, homotopyFactor);
			scene->onMeshLoaded(mesh);
			scene->registerMesh(mesh);
		}
	}

	PrimitivesPanel::PrimitivesPanel(const string& title, const TSharedPtr<Scene>& scene) : ImguiWindow(title)
	{
		m_scene = scene;
	}
	
    void PrimitivesPanel::onImGuiRender()
	{
        ImGui::SeparatorText("Mesh");

		static float homotopyFactor = 0.0f;
		ImGui::SliderFloat("Homotopy", &homotopyFactor, 0.0f, 1.0f);
		ImGui::SameLine();
		
		if (ImGui::Button("Apply"))
		{
			onHomotopyApply(static_cast<double>(homotopyFactor));
		}
	}
}