#pragma once 
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	class NodeEditor : public ImguiWindow
	{
	public:
		NodeEditor(const string& title);

		const ImGuiWindowFlags& flags() const override;
		
		void onImGuiRender() override;
		void update(const Timestep& ts);
	};
}