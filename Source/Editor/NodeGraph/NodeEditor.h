#pragma once
#define IMGUI_DEFINE_MATH_OPERATORS
#include "NodeGraph.h"
#include "../GUI/ImGuiWindow.h"

namespace VoxelEditor::nodes
{
	class ICanvasManipulator;

	class NodeEditor : public ImguiWindow
	{
	private:
		NodeGraph m_graph;
		std::vector<TSharedPtr<ICanvasManipulator>> m_manipulators;
	public:
		NodeEditor(const string& title);
		~NodeEditor() override = default;

		ImGuiWindowFlags flags() const override;
		
		void onBegin() override;
		void onImGuiRender() override;
	};
}
