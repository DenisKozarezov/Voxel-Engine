#include "NodeEditor.h"
#include "Manipulators/CanvasMoveManipulator.h"
#include "Manipulators/CanvasZoomManipulator.h"

namespace VoxelEditor::nodes
{
	NodeEditor::NodeEditor(const string& title) : ImguiWindow(title)
	{
		m_manipulators.push_back(MakeShared<CanvasMoveManipulator>(this));
		m_manipulators.push_back(MakeShared<CanvasZoomManipulator>(this));
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
		ImDrawList* drawList = ImGui::GetWindowDrawList();
		
		for (const auto& manipulator : m_manipulators)
		{
			manipulator->manipulateGraph(m_graph);
		}
		
		m_graph.onImGuiRender(drawList);
	}
}
