#include "Application.h"

namespace VoxelEngine
{
	GLFWwindow* const Application::createWindow(const int& width, const int& height, const string& title) const noexcept
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		return glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
	}

	Application::Application(const unsigned int& width, const unsigned int& height, const string& title)
	{
		_window = createWindow(width, height, title);
	}

	const void Application::init()
	{
		_renderer.setGLFWwindow(_window);

		try
		{
			_renderer.init();
		}
		catch (const std::exception& e)
		{
			std::cerr << e.what();
		}
	}
	const void Application::run()
	{
		_running = true;

		while (!glfwWindowShouldClose(_window))
		{
			glfwPollEvents();
			_renderer.renderFrame();
		}
		_renderer.deviceWaitIdle();
	}
	const void Application::shutdown()
	{
		_running = false;

		_renderer.cleanup();

		glfwDestroyWindow(_window);
		glfwTerminate();
	}

	void Application::onEvent(const input::Event& e)
	{
	}
	void Application::onWindowClosed(const input::WindowCloseEvent& e)
	{
		shutdown();
	}
	void Application::onWindowResized(const input::WindowResizeEvent& e)
	{
	}
}