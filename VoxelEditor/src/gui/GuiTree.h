#pragma once
#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	class GuiTree
	{
	private:
		std::vector<ImguiWindow*> m_windows;
		SceneViewport* m_viewport;
		
	public:
		GuiTree();

		FORCE_INLINE SceneViewport* getViewport() const { return m_viewport; }
		
		bool showDockSpace(bool *p_open) const;
		
		void registerWindow(ImguiWindow* window);
		void registerViewport(SceneViewport* viewport);
		void unregisterWindow(const ImguiWindow* window);
		void unregisterWindow(std::vector<ImguiWindow*>::iterator it);
		void onImGuiRender();
		void onUpdate(const Timestep& ts);
		void sendEvent(input::Event& e);

		~GuiTree() noexcept;
	};
}