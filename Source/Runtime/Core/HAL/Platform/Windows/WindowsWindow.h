#pragma once
#include <GLFW/glfw3.h>
#include <Core/Internal/Window.h>

namespace VoxelEngine
{
	class WindowsWindow : public Window
	{
	private:
		void init() override;
		void shutdown() override;

		struct WindowsOSData final
		{
			string Title;
			uint16 Width = 0, Height = 0;
			EventCallback EventCallback;
		};
		WindowsOSData m_windowData;
		GLFWwindow* m_window;
	public:
		WindowsWindow() = default;
		WindowsWindow(const WindowProperties&);

		FORCE_INLINE const uint16& getWidth() const noexcept override { return m_windowData.Width; }
		FORCE_INLINE const uint16& getHeight() const noexcept override { return m_windowData.Height; }
		FORCE_INLINE const void* getNativeWindow() const & noexcept override { return m_window; }
		FORCE_INLINE void setEventCallback(const EventCallback& callback) noexcept override
		{
			m_windowData.EventCallback = callback;
		}
		void setMaximized(const bool& isMaximized) override;
		void onUpdate() override;
		void waitEvents() const override;
		~WindowsWindow() override;
	};

	UniqueRef<Window> Window::Create(const WindowProperties& props)
	{
		return MakeUnique<WindowsWindow>(props);
	}
}
