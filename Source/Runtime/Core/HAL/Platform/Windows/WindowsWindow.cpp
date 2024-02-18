#include "WindowsWindow.h"
#include <Core/Logging/Assert.h>
#include <InputCore/events/ApplicationEvent.h>
#include <InputCore/events/KeyboardEvent.h>
#include <InputCore/events/MouseEvent.h>
#include <GLFW/glfw3.h>

namespace VoxelEngine
{
	static bool s_GLFWInitialized = false;

	WindowsWindow::WindowsWindow(const WindowProperties& props)
	{
		m_windowData.Title = props.Title;
		m_windowData.Width = props.Width;
		m_windowData.Height = props.Height;
		init();
	}

	void WindowsWindow::init()
	{
		RUNTIME_INFO("Creating window {0} ({1}, {2})...", m_windowData.Title, m_windowData.Width, m_windowData.Height);

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			RUNTIME_ASSERT(success, "Unable to initialize GLFW!");
			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		
		int success = glfwVulkanSupported();
		RUNTIME_ASSERT(success, "VulkanSDK Not Supported!");
		
		m_window = glfwCreateWindow(m_windowData.Width, m_windowData.Height, m_windowData.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, &m_windowData);

		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			data.Width = static_cast<uint16>(width);
			data.Height = static_cast<uint16>(height);
			
			input::WindowResizeEvent e(data.Width, data.Height);
			data.EventCallback(e);
		});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::WindowCloseEvent e;
			data.EventCallback(e);
		});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			const uint16 keycode = static_cast<uint16>(key);

			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			switch (action)
			{
			case GLFW_PRESS:
			{
				input::KeyPressedEvent e(keycode, false);
				data.EventCallback(e);
				break;
			}
			case GLFW_RELEASE:
			{
				input::KeyReleasedEvent e(keycode);
				data.EventCallback(e);
				break;
			}
			case GLFW_REPEAT:
			{
				input::KeyPressedEvent e(keycode, true);
				data.EventCallback(e);
				break;
			}
			}
		});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::KeyTypedEvent e(keycode);
			data.EventCallback(e);
		});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
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

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
		{
			WindowsOSData& data = *(WindowsOSData*)glfwGetWindowUserPointer(window);
			input::MouseMovedEvent e(static_cast<float>(xpos), static_cast<float>(ypos));
			data.EventCallback(e);
		});
	}
	void WindowsWindow::shutdown()
	{
		glfwDestroyWindow(m_window);
		m_window = nullptr;
	}
	void WindowsWindow::setMaximized(const bool& isMaximized)
	{
		m_maximized = isMaximized;
		if (isMaximized) 
			glfwMaximizeWindow(m_window);
	}
	void WindowsWindow::onUpdate()
	{
		glfwPollEvents();
	}
	void WindowsWindow::waitEvents() const
	{
		glfwPollEvents();
	}
	WindowsWindow::~WindowsWindow()
	{
		shutdown();
	}
}