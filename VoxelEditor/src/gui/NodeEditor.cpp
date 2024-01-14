#include "NodeEditor.h"

namespace VoxelEditor::gui
{
	NodeEditor::NodeEditor(const string& title) : ImguiWindow(title)
	{
	}

	const ImGuiWindowFlags& NodeEditor::flags() const
	{
		return ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	}

	void NodeEditor::onImGuiRender()
	{
		
	}

	void NodeEditor::update(const Timestep& ts)
	{
		
	}
}