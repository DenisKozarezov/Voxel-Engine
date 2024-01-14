#pragma once 
#include <VoxelEngine.h>

namespace VoxelEditor::gui
{
	class NodeEditor : public ImguiWindow
	{
	public:
		NodeEditor(const string& title);
		~NodeEditor() override = default;
		
		const ImGuiWindowFlags flags() const override;
		
		void onBegin() override;
		void onImGuiRender() override;
	};
}