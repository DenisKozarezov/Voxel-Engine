#pragma once
#include <VoxelEngine.h>

namespace VoxelEditor
{
	class EditorLayer : public VoxelEngine::renderer::Layer
	{
	private:
		const bool onKeyPressed(const VoxelEngine::input::KeyPressedEvent& e) const;
		const bool onMouseButtonPressed(const VoxelEngine::input::MouseButtonPressedEvent& e) const;
	public:
		EditorLayer();
		~EditorLayer() = default;

		void onAttach() override;
		void onDetach() override;
		void onUpdate(const VoxelEngine::Timestep& ts) override;
		void onImGuiRender() override;
		void onEvent(VoxelEngine::input::Event& e) override;
	};
}