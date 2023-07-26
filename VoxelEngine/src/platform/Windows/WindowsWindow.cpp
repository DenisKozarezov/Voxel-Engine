#include "WindowsWindow.h"
#include "core/Assert.h"
#include "core/input/events/ApplicationEvent.h"
#include "core/input/events/KeyboardEvent.h"
#include "core/input/events/MouseEvent.h"

namespace VoxelEngine
{
	static bool s_GLFWInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProperties& props)
	{
		_windowData.Title = props.Title;
		_windowData.Width = props.Width;
		_windowData.Height = props.Height;
		init();
	}

	void WindowsWindow::init()
	{
		VOXEL_CORE_INFO("Creating window {0} ({1}, {2})...", _windowData.Title, _windowData.Width, _windowData.Height)

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			VOXEL_CORE_ASSERT(success, "Unable to initialize GLFW!")
			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		_window = glfwCreateWindow(_windowData.Width, _windowData.Height, _windowData.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(_window);
		glfwSetWindowUserPointer(_window, &_windowData);	
		setVSync(true);

		glfwSetWindowSizeCallback(_window, [](GLFWwindow* window, int width, int height)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::WindowResizeEvent e(width, height);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS:
			{
				input::KeyPressedEvent e(key, false);
				data.EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				input::KeyReleasedEvent e(key);
				data.EventCallback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				input::KeyPressedEvent e(key, true);
				data.EventCallback(e);
				break;
			}
			}
		});

		glfwSetCharCallback(_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::KeyTypedEvent e(keycode);
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(_window, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
			case GLFW_PRESS:
			{
				input::MouseButtonPressedEvent e(button);
				data.EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				input::MouseButtonReleasedEvent e(button);
				data.EventCallback(e);
				break;
			}
			}
		});

		glfwSetCursorPosCallback(_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::MouseMovedEvent e(xpos, ypos);
			data.EventCallback(e);
		});
	}
	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(_window);
		_window = nullptr;
	}
	inline void WindowsWindow::setVSync(bool isEnabled) noexcept
	{
		glfwSwapInterval(isEnabled);
		_windowData.VSync = isEnabled;
	}
	inline void WindowsWindow::setEventCallback(const EventCallback& callback) noexcept
	{
		_windowData.EventCallback = callback;
	}
	void WindowsWindow::setMaximized(const bool& isMaximized)
	{
		if (isMaximized) 
			glfwMaximizeWindow(_window);
	}
	void WindowsWindow::onUpdate()
	{
		glfwPollEvents();
	}

	WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}
}