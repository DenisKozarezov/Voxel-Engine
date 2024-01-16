#pragma once
#include <Core/CoreMinimal.h>

namespace VoxelEditor::gui
{
	class PrimitivesPanel : public ImguiWindow
	{
	public:
		PrimitivesPanel(const string& title);
		~PrimitivesPanel() override = default;
		
		void onImGuiRender() override;
	};
}