#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	class PrimitivesPanel : public ImguiWindow
	{
	public:
		PrimitivesPanel(const string& title);

		void onImGuiRender() override;
	};
}