#pragma once
#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	class GuiTree : public NonCopyable
	{
	private:
		std::vector<ImguiWindow*> m_windows;
		SceneViewport* m_viewport = nullptr;
	public:
		GuiTree() noexcept = default;

		FORCE_INLINE SceneViewport* getViewport() const { return m_viewport; }
		
		bool showDockSpace(bool *p_open) const;
		
		bool registerWindow(ImguiWindow* window);
		bool registerViewport(SceneViewport* viewport);
		void unregisterWindow(const ImguiWindow* window);
		void unregisterWindow(std::vector<ImguiWindow*>::iterator it);
		void onImGuiRender();
		void onUpdate(const Timestep& ts);
		void sendEvent(input::Event& e);

		~GuiTree() noexcept;
	};
}