#pragma once
#include "gui/GuiTree.h"

namespace VoxelEditor::gui
{
	class EditorLayer : public renderer::Layer
	{
	private:
		GuiTree m_guiTree;
		SharedRef<components::mesh::Mesh> m_loadedMesh;
		SharedRef<Scene> m_scene;

		void loadModel();
		void saveLayout();
		void drawMenuBar();
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void onAttach() override;
		void onDetach() override;
		void onUpdate(const Timestep& ts) override;
		void onFixedUpdate(const Timestep& ts) override;
		void onImGuiRender() override;
		void onEvent(input::Event& e) override;
	};
}