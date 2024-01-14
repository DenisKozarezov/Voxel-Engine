#include "NodeEditor.h"

namespace VoxelEditor::gui
{
	NodeEditor::NodeEditor(const string& title) : ImguiWindow(title)
	{
	}

	const ImGuiWindowFlags NodeEditor::flags() const
	{
		return ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	}

	void NodeEditor::onBegin()
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
	}
	void NodeEditor::onImGuiRender()
	{
		auto windowPos = ImGui::GetWindowPos();
		auto cursorPos = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(windowPos, cursorPos, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0, 0, 1.0f)), 3.0f);
	}
}