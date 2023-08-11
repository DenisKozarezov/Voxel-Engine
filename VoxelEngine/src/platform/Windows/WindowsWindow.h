#pragma once
#include <GLFW/glfw3.h>
#include <core/Window.h>

namespace VoxelEngine
{
	class WindowsWindow : public Window
	{
	private:
		virtual void init() override;
		virtual void shutdown() override;

		struct WindowsOSData final
		{
			string Title;
			uint16 Width = 0, Height = 0;
			bool VSync = false;
			EventCallback EventCallback;
		};
		WindowsOSData m_windowData;
		GLFWwindow* m_window;
	public:
		WindowsWindow() = default;
		WindowsWindow(const WindowProperties&);

		inline const uint16& getWidth() const noexcept override { return m_windowData.Width; }
		inline const uint16& getHeight() const noexcept override { return m_windowData.Height; }
		inline const bool& VSyncEnabled() const noexcept override { return m_windowData.VSync; }
		const void* getNativeWindow() const & noexcept override { return m_window; }
		void setVSync(bool isEnabled) noexcept override;
		void setEventCallback(const EventCallback& callback) noexcept override;
		void setMaximized(const bool& isMaximized) override;
		void onUpdate() override;
		void waitEvents() const override;
		~WindowsWindow();
	};

	UniqueRef<Window> Window::Create(const WindowProperties& props)
	{
		return MakeUnique<WindowsWindow>(props);
	}
}
