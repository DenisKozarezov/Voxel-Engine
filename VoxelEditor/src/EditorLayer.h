#pragma once
#include "SceneView.h"
#include "EditorConsole.h"

namespace VoxelEditor
{
	class EditorLayer : public renderer::Layer
	{
	private:
		input::EventDispatcher m_dispatcher;
		Scene m_scene;
		SceneView m_sceneView;
		EditorConsole m_console;
		float m_deltaTime = 0.0f;
		float m_fixedDeltaTime = 0.0f;

		bool onKeyboardPressed(const input::KeyPressedEvent& e);

		void drawMenuBar();
		void drawRenderPerformance();
	public:
		EditorLayer();
		~EditorLayer() = default;

		void onAttach() override;
		void onDetach() override;
		void onUpdate(const Timestep& ts) override;
		void onFixedUpdate(const Timestep& ts) override;
		void onImGuiRender() override;
		void onEvent(input::Event& e) override;
	};
}