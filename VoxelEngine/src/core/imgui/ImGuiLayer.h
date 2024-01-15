#pragma once
#include <imgui.h>
#include <imgui_internal.h>
#include "../Layer.h"

namespace VoxelEngine::renderer
{
	class ImGuiLayer : public renderer::Layer
	{
	private:
		bool m_blockEvents = true;

		void loadLayout();

		void onAttach() override;
		void onDetach() override;
		void onEvent(input::Event& e) override;
	public:
		enum ColorStyle : byte { Dark, Light, Classic };

		ImGuiLayer() : Layer("ImGUILayer") { }

		void preRender();
		void postRender();
		void setStyle(const ColorStyle& style) const;
		void saveLayout(const string& filepath);
		FORCE_INLINE void blockEvents(const bool& block) { m_blockEvents = block; }

		~ImGuiLayer() override = default;
	};
}
