#pragma once 
#include "ImGuiWindow.h"
#include "../NodeGraph/NodeGraph.h"

namespace VoxelEditor::gui
{
	class NodeEditor : public ImguiWindow
	{
	private:
		nodes::NodeGraph m_graph;
	public:
		NodeEditor(const string& title);
		~NodeEditor() override = default;

		ImGuiWindowFlags flags() const override;
		
		void onBegin() override;
		void onImGuiRender() override;
	};
}