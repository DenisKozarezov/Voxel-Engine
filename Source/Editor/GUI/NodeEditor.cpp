#include "NodeEditor.h"

namespace VoxelEditor::gui
{
	NodeEditor::NodeEditor(const string& title) : ImguiWindow(title)
	{

	}

	ImGuiWindowFlags NodeEditor::flags() const
	{
		return ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;
	}

	void NodeEditor::onBegin()
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize(ImVec2(1000.0f, 700.0f));

		const ImVec2 viewportSize = ImGui::GetMainViewport()->Size;
		ImGui::SetNextWindowPos(ImVec2(viewportSize.x * 0.5f, viewportSize.y * 0.5f), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	}
	void NodeEditor::onImGuiRender()
	{
		const ImVec2 windowPos = ImGui::GetWindowPos();
		const ImVec2 cursorPos = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddLine(windowPos, cursorPos, ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0, 0, 1.0f)), 3.0f);
	}
}