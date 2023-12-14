#pragma once
#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	class GuiTree
	{
	private:
		std::vector<ImguiWindow*> m_windows;
		SharedRef<SceneViewport> m_viewport;
	public:
		GuiTree();

		INLINE SharedRef<SceneViewport> getViewport() const { return m_viewport; }

		void registerWindow(ImguiWindow* window);
		void registerViewport(const SharedRef<SceneViewport>& viewport);
		void onImGuiRender();

		~GuiTree() noexcept;
	};
}