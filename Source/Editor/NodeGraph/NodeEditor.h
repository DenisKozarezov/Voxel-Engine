#pragma once
#include "NodeGraph.h"
#include "Manipulators/ICanvasManipulator.h"
#include "../GUI/ImGuiWindow.h"

namespace VoxelEditor::nodes
{
	class NodeEditor : public ImguiWindow
	{
	private:
		NodeGraph m_graph;
		std::vector<TUniquePtr<ICanvasManipulator>> m_manipulators;
	public:
		NodeEditor(const string& title);
		~NodeEditor() override = default;

		ImGuiWindowFlags flags() const override;
		
		void onBegin() override;
		void onImGuiRender() override;
	};
}
