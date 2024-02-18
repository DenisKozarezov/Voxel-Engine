#pragma once
#include "SceneViewport.h"

namespace VoxelEditor::gui
{
	struct window_find_pred
	{
		string key;
		window_find_pred(const string& key): key(key) {}
		window_find_pred(string&& key): key(std::move(key)) {}
		
		bool operator()(TSharedPtr<ImguiWindow>& i) const {
			return i->title() == key;
		}
	};
	
	class GuiTree : public NonCopyable
	{
	private:
		std::vector<TSharedPtr<ImguiWindow>> m_windows;
		TSharedPtr<SceneViewport> m_viewport = nullptr;
	public:
		GuiTree() noexcept = default;

		FORCE_INLINE TWeakPtr<SceneViewport> getViewport() const { return m_viewport; }
		
		bool showDockSpace(bool *p_open) const;
		
		bool registerWindow(const TSharedPtr<ImguiWindow>& window);
		bool registerViewport(const TSharedPtr<SceneViewport>& viewport);
		void unregisterWindow(TSharedPtr<ImguiWindow>& window);
		void onImGuiRender();
		void onUpdate(const Timestep& ts);
		void sendEvent(input::Event& e);

		~GuiTree() = default;
	};
}