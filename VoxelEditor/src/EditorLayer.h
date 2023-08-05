#pragma once
#include "SceneView.h"

namespace VoxelEditor
{
	class EditorLayer : public renderer::Layer
	{
	private:
		input::EventDispatcher _dispatcher;
		Scene _scene;
		SceneView _sceneView;
		float _deltaTime = 0.0f;
		float _fixedDeltaTime = 0.0f;

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