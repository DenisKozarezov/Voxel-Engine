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
			uint16 Width, Height;
			bool VSync;
			EventCallback EventCallback;
		};
		WindowsOSData _windowData;
		GLFWwindow* _window;
	public:
		WindowsWindow() = default;
		WindowsWindow(const WindowProperties&);

		inline const uint16& getWidth() const noexcept override { return _windowData.Width; }
		inline const uint16& getHeight() const noexcept override { return _windowData.Height; }
		inline const bool& VSyncEnabled() const noexcept override { return _windowData.VSync; }
		inline const void* getNativeWindow() const override { return _window; }
		void setVSync(bool isEnabled) noexcept override;
		void setEventCallback(const EventCallback& callback) noexcept override;
		void onUpdate() override;

		~WindowsWindow();
	};

	UniqueRef<Window> Window::Create(const WindowProperties& props)
	{
		return MakeUnique<WindowsWindow>(props);
	}
}
