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
		const bool showDockSpace(bool *p_open) const;
		void onImGuiRender();
		void onUpdate(const Timestep& ts);

		~GuiTree() noexcept;
	};
}