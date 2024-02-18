#pragma once
#include <Core/HAL/Platform/Window.h>

class GLFWwindow;

namespace VoxelEngine
{
	class WindowsWindow : public Window
	{
	private:		
		void init() override;

		struct WindowsOSData final
		{
			string Title;
			uint16 Width = 0, Height = 0;
			EventCallback EventCallback;
		};
		WindowsOSData m_windowData;
		GLFWwindow* m_window;
		bool m_maximized = false;
	public:
		WindowsWindow() = default;
		WindowsWindow(const WindowProperties&);

		FORCE_INLINE const uint16& getWidth() const noexcept override { return m_windowData.Width; }
		FORCE_INLINE const uint16& getHeight() const noexcept override { return m_windowData.Height; }
		FORCE_INLINE const void* getNativeWindow() const & noexcept override { return m_window; }
		FORCE_INLINE bool isMaximized() const override { return m_maximized; }
		FORCE_INLINE void setEventCallback(const EventCallback& callback) noexcept override
		{
			m_windowData.EventCallback = callback;
		}
		void setMaximized(const bool& isMaximized) override;
		void onUpdate() override;
		void waitEvents() const override;
		void shutdown() override;
		~WindowsWindow() override;
	};

	TUniquePtr<Window> Window::Create(const WindowProperties& props)
	{
		return MakeUnique<WindowsWindow>(props);
	}
}
