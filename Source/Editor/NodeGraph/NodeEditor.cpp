#include "NodeEditor.h"
#include "Manipulators/CanvasMoveManipulator.h"
#include "Manipulators/CanvasZoomManipulator.h"
#include "Manipulators/CanvasSelectRectManipulator.h"

namespace VoxelEditor::nodes
{
	NodeEditor::NodeEditor(const string& title) : ImguiWindow(title)
	{
		m_manipulators.push_back(MakeUnique<CanvasMoveManipulator>(this));
		m_manipulators.push_back(MakeUnique<CanvasZoomManipulator>(this));
		m_manipulators.push_back(MakeUnique<CanvasSelectRectManipulator>(this));
	}

	ImGuiWindowFlags NodeEditor::flags() const
	{
		return ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
	}

	void NodeEditor::onBegin()
	{
		ImGui::SetNextWindowBgAlpha(1.0f);
		ImGui::SetNextWindowSize(ImVec2(1200.0f, 900.0f));

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
