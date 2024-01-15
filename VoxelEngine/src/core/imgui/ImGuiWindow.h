#pragma once
#include <imgui.h>
#include <core/Base.h>
#include <core/PrimitiveTypes.h>
#include <core/Timestep.h>
#include <core/input/events/EventDispatcher.h>
#include <core/NonCopyable.h>

namespace VoxelEngine
{
	class ImguiWindow : public NonCopyable
	{
	private:
		bool m_visible = true;
		bool m_hovered = false;
		float m_minSize[2] = { 200.0f, 400.0f };
		float m_maxSize[2] = { 1920.0f, 1080.0f };
		string m_title;

		input::EventDispatcher m_eventDispatcher;
	public:
		ImguiWindow(const string& title = "Window");
		virtual ~ImguiWindow() = default;

		FORCE_INLINE const bool& isVisible() const { return m_visible; }
		FORCE_INLINE const bool& isHovered() const { return m_hovered; }
		FORCE_INLINE const string& title() const { return m_title; }
		INLINE virtual ImGuiWindowFlags flags() const { return 0; }
		virtual bool hasToolbar() const { return false; }
		virtual bool wantCaptureKeyboard() const { return false; }
		virtual bool wantCaptureMouse() const { return false; }

		bool begin();
		void end();
		void setVisibility(const bool& isVisible);
		void setMinSize(const float& minWidth, const float& minHeight);
		void show();
		void hide();

		template<typename TEvent>
		void sendEvent(TEvent& e);

		template<typename TEvent, typename TFunc>
		requires input::is_event_func<TEvent, TFunc>
		constexpr void subscribeEvent(TFunc callback);
		
		template<typename TEvent>
		requires input::is_event<TEvent>
		void unsubscribeEvent();

		virtual void onImGuiRender() = 0;
		virtual void onBegin() { }
		virtual void onEnd() { }
		virtual void onToolbar(const bool& hovered) { }
		virtual void update(const Timestep& ts) { }
	};

	template<typename TEvent>
	INLINE void ImguiWindow::sendEvent(TEvent& e)
	{
		m_eventDispatcher.dispatchEvent(e);
	}

	template<typename TEvent, typename TFunc>
	requires input::is_event_func<TEvent, TFunc>
	INLINE constexpr void ImguiWindow::subscribeEvent(TFunc callback)
	{
		m_eventDispatcher.registerEvent<TEvent, TFunc>(callback);
	}

	template<typename TEvent>
	requires input::is_event<TEvent>
	INLINE void ImguiWindow::unsubscribeEvent()
	{
		m_eventDispatcher.unregisterEvent<TEvent>();
	}
}
