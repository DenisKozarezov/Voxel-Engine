#pragma once
#include <imgui.h>
#include "../Layer.h"

namespace VoxelEngine::renderer
{
	class ImGuiLayer : public renderer::Layer
	{
	private:
		bool _blockEvents = true;

		void onAttach() override;
		void onDetach() override;
		void onEvent(input::Event& e) override;

		inline void blockEvents(const bool& block) { _blockEvents = block; }
	public:
		ImGuiLayer() : Layer("ImGUILayer") { }
		~ImGuiLayer() = default;
	};
}
