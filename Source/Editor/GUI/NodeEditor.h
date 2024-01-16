#pragma once 
#include <Core/CoreMinimal.h>

namespace VoxelEditor::gui
{
	class NodeEditor : public ImguiWindow
	{
	public:
		NodeEditor(const string& title);
		~NodeEditor() override = default;

		ImGuiWindowFlags flags() const override;
		
		void onBegin() override;
		void onImGuiRender() override;
	};
}