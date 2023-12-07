#pragma once
#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	class GuiTree
	{
	private:
		std::vector<ImGuiWindow*> m_windows;
		SceneViewport* m_viewport;
	public:
		GuiTree();

		INLINE SceneViewport* getViewport() const { return m_viewport; }

		void registerWindow(ImGuiWindow* window);
		void registerViewport(SceneViewport* viewport);
		void unregisterWindow(ImGuiWindow* window);
		void onImGuiRender();

		~GuiTree() noexcept = default;
	};
}