#pragma once
#include "ImGuiWindow.h"

namespace VoxelEditor::gui
{
	class PrimitivesPanel : public ImguiWindow
	{
	private:
		TWeakPtr<Scene> m_scene;
		void onHomotopyApply(const double& homotopyFactor);
	public:
		PrimitivesPanel(const string& title, const TSharedPtr<Scene>& scene);
		~PrimitivesPanel() override = default;
		
		void onImGuiRender() override;
	};
}